(**********************************************************)
(*                PROCEDUR UPDATE                         *)
(**********************************************************)
OVERLAY PROCEDURE UPDATE;
CONST
  PRO1 = 'STICHWORT "" NEU. AUFNEHMEN (Y/N)?';
VAR
  RZAHL         :  REAL;
  RECNR,ANZ,
  RECNRALT      :  INTEGER;
  CH            :  CHAR;
  KEY           :  KEYREC;
  KEYREF,I,A,B  :  INTEGER;
  ALTKEY        :  KW;
  REPCOND       :  BOOLEAN;
  INKEY         :  ARRAY[1..ANZGES] OF BYTE;
  PROMPT        :  STR80;
  NUM           :  STR2;
  NUMDSK        :  STR3;
  EINTRAGINT    :  INTEGER;
  ZAHL          :  INTEGER;
  L_OK          :  BOOLEAN;
  TEMPSTR       :  STRING[5];
  AUTOR_GES     :  STRING[142];
  AUTOR_STR     :  STR80;
  (**)

PROCEDURE OUTSCR(VAR SCR : SCREENRECEXT);
PROCEDURE TUE(VAR ST    : STR80;
                  X,Y,L : INTEGER);
VAR
   I : INTEGER;
BEGIN
   GOTOXY(X,Y);WRITE(ST,'':L - LENGTH(ST));
END;
BEGIN
   WITH SCR DO
   BEGIN
      IF C.ABS <> 0 THEN
      BEGIN
         GOTOXY(33,4);WRITE(C.ABS:6);
      END;
      TUE(C.S_FELD,53,4,4);
      GOTOXY(70,4);WRITE(C.NR:5);
      C.TITEL1[0]:= CHR(LENGTH(C.TITEL1) - 2);
      TUE(C.TITEL1,8,6,72);
      C.TITEL1[0]:= CHR(LENGTH(C.TITEL1) + 2);
      GOTOXY(80,6);
      IF C.TITEL1[LENGTH(C.TITEL1) - 1] = #$8D THEN
         WRITE(#$BA)
      ELSE
         WRITE(#$FE);
      TUE(C.TITEL2,8,7,72);
      GOTOXY(80,7);
      WRITE(#$FE);
      C.AUTOR_A[0]:= CHR(LENGTH(C.AUTOR_A) - 2);
      TUE(C.AUTOR_A,10,9,70);
      C.AUTOR_A[0]:= CHR(LENGTH(C.AUTOR_A) + 2);
      GOTOXY(80,9);
      IF C.AUTOR_A[LENGTH(C.AUTOR_A) - 1] = #$8D THEN
         WRITE(#$BA)
      ELSE
         WRITE(#$FE);
      TUE(C.AUTOR_B,10,10,70);
      GOTOXY(80,10);
      WRITE(#$FE);
      CASE ART OF
         'Z' : BEGIN
                  TUE(C.ZEITSCHR,14,12,50);
                  TUE(C.BAND,14,14,20);
               END;
         'B' : BEGIN
                  TUE(C.BVERLAG,14,12,60);
                  TUE(C.BORT,14,14,60);
               END;
         'S' : BEGIN
                  TUE(C.HRSG,14,12,60);
                  TUE(C.HRSGTITEL,14,13,60);
                  TUE(C.SVERLAG,14,14,60);
                  TUE(C.SORT,14,15,60);
               END;
          END;     (* END CASE ART OF *)
      TUE(C.JAHR,26,16,12);
      TUE(C.SEITE,48,16,12);
      FOR I:= 1 TO 12 DO
         TUE(S[I],((I - 1)MOD 3) * 26 + 2,(I - 1) DIV 3 + 18,URKEYLEN);
   END;   (* END WITH SCR DO *)
END;      (* END PROCEDURE OUTSCR *)

PROCEDURE OUTMASK(ART : CHAR);
VAR X,Y : INTEGER;
BEGIN
   GOTOXY(1,4);
   IF ART = 'Z' THEN
      WRITE  ('>> ZEITSCHRIFT <<');
   IF ART = 'S' THEN
      WRITE  ('>> SAMMELBAND << ');
   IF ART = 'B' THEN
      WRITE  ('>> BUCH <<       ');

   GOTOXY(20,4);WRITE('ABSTRACT-NR.:');
   GOTOXY(42,4);WRITE('SORT-FELD :');
   GOTOXY(60,4);WRITE('TITEL-NR.:');
   GOTOXY(1,6);WRITE('TITEL :');
   GOTOXY(1,9);WRITE('AUTOREN :');

   GOTOXY(1,12);
   CASE ART OF
      'Z'   :  BEGIN
                  WRITE               ('ZEITSCHRIFT :');
                  GOTOXY(1,13);CLREOL;
                  GOTOXY(1,14);WRITE  ('       BAND :');
                  GOTOXY(1,15);CLREOL;
               END;
      'S'   :  BEGIN
                  WRITE               ('HERAUSGEBER :');
                  GOTOXY(1,13);WRITE  ('(HRSG)TITEL :');
                  GOTOXY(1,14);WRITE  ('     VERLAG :');
                  GOTOXY(1,15);WRITE  ('        ORT :');
               END;
      'B'   :  BEGIN
                  WRITE               ('     VERLAG :');
                  GOTOXY(1,13);CLREOL;
                  GOTOXY(1,14);WRITE  ('        ORT :');
                  GOTOXY(1,15);CLREOL;
               END;
           END;

   GOTOXY(20,16); WRITE('JAHR :');
   GOTOXY(41,16); WRITE('SEITE :');

   GOTOXY(1,17);WRITE('STICHWORTE :');
   FOR Y:= 18 TO 21 DO
      FOR X:= 0 TO 2 DO
      BEGIN
         GOTOXY(X * 26 + 1,Y); WRITE(#$B4);
      END;
END;

PROCEDURE ABSTR(VAR FIRSTREC : INTEGER;
                     TIT_NR  : INTEGER);
VAR
   ABSNAME : STRING[34];

BEGIN            (* PROCEDURE ABSTR *)
   IF OK_1 = FALSE THEN
   BEGIN
      BEEP;
      EXIT;
   END;
   ABSNAME:= PR.TITPFAD;
   ABSNAME:= ABSNAME + '.ABS';               (* EXTENSION .ABS ANFUEGEN *)
   IF NOT ABS_OPEN THEN
   BEGIN
      OPENFILE(ABSF,ABSNAME,256);
      IF NOT OK THEN
      BEGIN
         SELECT('ABSTRACT DATEI FEHLT. CREATE (Y/N)?',['Y','N'],A_CH,' ');
         IF A_CH = 'Y' THEN
            MAKEFILE(ABSF,ABSNAME,256)
         ELSE EXIT;
      END;
      ABS_OPEN:= TRUE;
      GETFREEGROUPS;
      CHECKFREEGROUPS;
      IF NOT ENOUGH THEN
      BEGIN
         OK_1:= FALSE;
         OUTMASK(ART);
         OUTSCR(SCR);
         EXIT;
      END;
   END;
   CLEARFRAME;
   REPEAT
   SELECT('ABSTRACT: B)EARBEITEN, L)OESCHEN, Q)UIT',['B','L','Q',#$0D],A_CH,'B');
     IF A_CH = 'L' THEN
     BEGIN
        SELECT('LOESCHEN ? SIND SIE SICHER (Y/N)?',['Y','N',#$0D],A_TC,'N');
        IF A_TC = 'Y' THEN
           IF FIRSTREC <> 0 THEN
           BEGIN
              LOESCHE(FIRSTREC);
              FIRSTREC:= 0;
           END;
      END;
      IF A_CH = 'Q' THEN
      BEGIN
         CLEARFRAME;
         OUTMASK(ART);
         OUTSCR(SCR);
         EXIT;
      END;
   UNTIL A_CH = 'B';
   GOTOXY(1,23);
   WRITE('ABSTRACT BEARBEITEN:  BEENDEN MIT <END>');CLREOL;
   IF FIRSTREC = 0 THEN
      NEU(FIRSTREC,TIT_NR);
   INIT_A_PTR;                   (* POINTER UND HEAP INITIALISIEREN *)
   INPUT(FIRSTREC,79);           (* TEXT AUS DATEI ABSF EINLESEN *)
   INITSCREEN(4,21,0,79,'.');    (* PARAMETER SIND: 1.ZEILE, LETZTE ZEILE, *)
                                 (* 1.ZEICHEN IN ZEILE, ZEILENLAENGE, BLANK *)
   ACT_SCREEN(4,21,0,79,'.');
   INPUTSCREEN(4,21,0,79,6,'.',A_TC,FALSE);
   OUT_DISK(FIRSTREC,TIT_NR);
   RELEASE(A_HEAPTOP);
   CLEARFRAME;
   OUTMASK(ART);
   OUTSCR(SCR);
END;

PROCEDURE INPUTCUST(VAR SCR : SCREENRECEXT);
CONST
   TERM  : CHARSET = [#$02,#$0C,#$05,#$09,#$0D,#$18,#$1A];
   OUTST = 'ADD/EDIT: BEENDEN <END> ; ABSTRACT <CTRL-L> ODER <F1>';
VAR
   L,P          : INTEGER;
   TC,L_CH      : CHAR;
   EINGANGS_NR,
   ABSTRACT_NR,
   AUSGANGS_NR  : INTEGER;
   OK           : BOOLEAN;
   TEMPSTR,
   TEMPSTR1     : STRING[5];

PROCEDURE MODY_2STR(VAR S1      : ANYSTR;
                    VAR S2      : ANYSTR;
                        SCR_ANF,
                        SCR_END,
                        Z_ANF,
                        Z_LEN   : INTEGER;
                    VAR A_TC    : CHAR);
VAR
   SUM_STR    : STRING[160];
   CNT,INP    : INTEGER;
   TXTEND     : BOOLEAN;

BEGIN    (* PROCEDURE MODY_2STR *)
   CNT:= 1;
   INP:= 1;
   TXTEND:= FALSE;
   SUM_STR:= S1 + S2 + #$1A;
   INIT_A_PTR;
   INPUT_REC(SUM_STR[1],149,72,CNT,INP,TXTEND);
   READ_OK:= TRUE;
   INITSCREEN(SCR_ANF,SCR_END,Z_ANF,Z_LEN,' ');
   ACT_SCREEN(SCR_ANF,SCR_END,Z_ANF,Z_LEN,' ');
   INPUTSCREEN(SCR_ANF,SCR_END,Z_ANF,Z_LEN,6,'.',A_TC,TRUE);
   A_AKTPTR:= A_SCREENPTR;
   GOTOXY(Z_ANF + 1,SCR_ANF);WRITE(A_AKTPTR^.LINE);CLREOL;
   GOTOXY(80,SCR_ANF);WRITE(A_AKTPTR^.RTN);
   GOTOXY(Z_ANF + 1,SCR_END);
   S2:= '';
   IF A_AKTPTR^.NEXT <> NIL THEN
   BEGIN
      S2:= A_AKTPTR^.NEXT^.LINE;
      WRITE(A_AKTPTR^.NEXT^.LINE);
   END;
   CLREOL;
   GOTOXY(80,SCR_END);WRITE(#$FE);
   S1:= A_AKTPTR^.LINE;
   IF (A_AKTPTR^.RTN = #$FE) AND (S2 <> '') THEN
      S1:= S1 + #$0D#$0A
   ELSE
      S1:= S1 + #$8D#$0A;
   RELEASE(A_HEAPTOP);
END;  (* PROCEDURE MODY_2STR *)

BEGIN       (* PROCEDURE INPUTCUST *)
   L:= 1;
   P:= 0;
   ABSTRACT_NR:= 0;
   TEMPSTR:= SCR.C.NR;
   GOTOXY(1,23);WRITE(OUTST);CLREOL;
   STR5_TO_INTEGER(SCR.C.NR,EINGANGS_NR,OK,TEMPSTR1,RZAHL);
   IF NOT OK THEN
      EINGANGS_NR:= 0;
   WITH SCR DO
   REPEAT
     CASE L OF
       0 : INPUTSTR(C.S_FELD,4,52,3,TERM,TC,2,P,'.',US_T);
       1 : INPUTSTR(C.NR,5,69,3,TERM,TC,2,P,'.',US_T);
       2 : MODY_2STR(C.TITEL1,C.TITEL2,6,7,7,72,TC);
       3 : MODY_2STR(C.AUTOR_A,C.AUTOR_B,9,10,9,70,TC);
       4 : CASE ART OF
              'Z' : INPUTSTR(C.ZEITSCHR,50,13,11,TERM,TC,5,P,'.',US_T);
              'B' : INPUTSTR(C.BVERLAG,60,13,11,TERM,TC,5,P,'.',US_T);
              'S' : INPUTSTR(C.HRSG,60,13,11,TERM,TC,5,P,'.',US_T);
           END;
       5 : IF ART = 'S' THEN
              INPUTSTR(C.HRSGTITEL,60,13,12,TERM,TC,5,P,'.',US_T);
       6 : CASE ART OF
              'Z' : INPUTSTR(C.BAND,20,13,13,TERM,TC,5,P,'.',US_T);
              'B' : INPUTSTR(C.BORT,60,13,13,TERM,TC,5,P,'.',US_T);
              'S' : INPUTSTR(C.SVERLAG,60,13,13,TERM,TC,5,P,'.',US_T);
           END;
       7 : IF ART = 'S' THEN
              INPUTSTR(C.SORT,60,13,14,TERM,TC,5,P,'.',US_T);
       8 : INPUTSTR(C.JAHR,12,25,15,TERM,TC,5,P,'.',US_T);
       9 : INPUTSTR(C.SEITE,12,47,15,TERM,TC,5,P,'.',US_T);
  10..21 : INPUTSTR(S[L - 9],URKEYLEN,((L - 10)MOD 3)*26+1,
                    (L - 10)DIV 3 + 17,TERM,TC,3,P,'.',US_T);



     END;
     IF (TC = #$09) OR (TC = #$0D) OR (TC = #$18) THEN (* TAB, CR, L-DOWN *)
         IF L = 21 THEN
            L:= 0
         ELSE L:= L + 1
     ELSE
       IF TC = #$05 THEN    (* CTRL-E = LINE UP *)
          IF L = 0 THEN
             L:=21
          ELSE L:= L - 1;

     IF TC = #$0C THEN      (* EINSPRUNG IN ABSTRACT UNTERPROGRAMM *)
     BEGIN
        STR5_TO_INTEGER(SCR.C.NR,ABSTRACT_NR,OK,TEMPSTR1,RZAHL);
        IF NOT OK THEN
           ABSTRACT_NR:= 0;
        ABSTR(C.ABS,ABSTRACT_NR);
        GOTOXY(33,4);WRITE(C.ABS:6);
        GOTOXY(1,23);WRITE(OUTST);CLREOL;
     END
     ELSE
        P:= 0;

   UNTIL (TC = #$0D) AND (L = 0) OR (TC = #$1A) OR (TC = #$02);
                             (* CR , CTRL-Z, END *)
   STR5_TO_INTEGER(SCR.C.NR,AUSGANGS_NR,OK,TEMPSTR1,RZAHL);
   IF NOT OK THEN
      AUSGANGS_NR:= 0;
   IF SCR.C.ABS <> 0 THEN       (* WENN ABSTRACT-DATEI VORHANDEN *)
   BEGIN
      IF (EINGANGS_NR <> ABSTRACT_NR) OR (EINGANGS_NR <> AUSGANGS_NR) OR
         (ABSTRACT_NR <> AUSGANGS_NR) THEN
      BEGIN
         IF NOT ABS_OPEN THEN
         BEGIN
            OPENFILE(ABSF,PR.TITPFAD + '.ABS',256);
            IF NOT OK THEN
            BEGIN
               GOTOXY(1,23);
               WRITE('ABSRACT-DATEI FEHLT. TITEL-NR ',
                     'AUF ALTEN WERT VON ',TEMPSTR,' GESETZT.');
               CLREOL;
               READ(KBD,L_CH);
               SCR.C.NR:= TEMPSTR;
            END
            ELSE ABS_OPEN:= TRUE;
         END;
         IF ABS_OPEN THEN
         BEGIN
            INIT_A_PTR;
            INPUT(SCR.C.ABS,79);
            OUT_DISK(SCR.C.ABS,AUSGANGS_NR);
            RELEASE(A_HEAPTOP);
         END;
      END;       (* END WENN EIN- UND AUSGANGSNUMMERN UNGLEICH *)
   END;          (* END WENN C.ABSF <> 0 *)
END;

FUNCTION INTOSTR(N: INTEGER) : STR2;  (* WANDELT EINE INTEGER ZAHL *)
BEGIN                                 (* IN STRING DER LAENGE 2 UM *)
  N:= N + $8000;
  INTOSTR:=CHR(HI(N))+CHR(LO(N));
END;
  (**)
FUNCTION STRTOINT(S: STR2) : INTEGER; (* WANDELT STRING IN INTEGER *)
BEGIN
  STRTOINT:= SWAP(ORD(S[1])) + ORD(S[2]) + $8000;
END;

PROCEDURE MAKE_AUTOR_KEYS;
VAR
   I,FIRST_SL,
   SEC_SL       : INTEGER;
BEGIN
   AUTOR_GES:= SCR.C.AUTOR_A;
   AUTOR_GES[0]:= CHR(LENGTH(AUTOR_GES) - 2);
   AUTOR_GES:= AUTOR_GES + SCR.C.AUTOR_B;
   FIRST_SL:= 1;
   I:= 1;
   REPEAT
      SEC_SL:= POS('/',AUTOR_GES);
      IF SEC_SL <> 0 THEN
      BEGIN
         AUTOR_GES[SEC_SL]:= ' ';
         AUTOR_STR:= COPY(AUTOR_GES,FIRST_SL,SEC_SL - FIRST_SL);
         FIRST_SL:= SEC_SL + 1;
      END
      ELSE
      BEGIN
         AUTOR_STR:= COPY(AUTOR_GES,FIRST_SL,
                          LENGTH(AUTOR_GES) - FIRST_SL + 1);
      END;

      REPEAT           (* LOESCHEN VON DEN SLASH UMGEBENDEN BLANKS *)
         IF AUTOR_STR[1] = ' ' THEN
            DELETE(AUTOR_STR,1,1);
      UNTIL (AUTOR_STR[1] <> ' ') OR (LENGTH(AUTOR_STR) = 0);
      REPEAT
         IF AUTOR_STR[LENGTH(AUTOR_STR)] = ' ' THEN
            AUTOR_STR[0]:= CHR(ORD(AUTOR_STR[0]) - 1);
      UNTIL (AUTOR_STR[LENGTH(AUTOR_STR)] <> ' ') OR (LENGTH(AUTOR_STR) = 0);

      MAKEKEY(AUTOR_STR,SCR.S[I + ANZKEY], 1);
      I:= I + 1;
   UNTIL (I = (ANZATR + 1)) OR (SEC_SL = 0);
END;

  (***************************************************************)
  (**)   (* TRAEGT TITEL IN VERWEIS-DATEI EIN (DAS STICHWORT IST *)
  (**)   (* SCHON BEKANNT) *)
  (**)
PROCEDURE PUTVERWEIS (VAR RECNR,ANZ : INTEGER);
VAR                  (* RECNR ENTSPR. STICHWORT, ANZ ENTSPR. NUMMER *)
  A : INTEGER;       (* DES TITELEINTRAGS IN DER DATEI DATF *)
BEGIN
   REPEAT
     GETREC(VERFILE,RECNR,VERWEIS);  (* BEGINN DER ZU DEM BETR. STICHWORT *)
     RECNRALT:= RECNR;
     RECNR:=VERWEIS.FOLGE;           (* GEHOERIGEN LISTE SUCHEN *)
   UNTIL RECNR = 0;
   A:=0;
   REPEAT
     A:=A + 1;
   UNTIL VERWEIS.DVR[A] = 0;
   VERWEIS.DVR[A]:=ANZ;      (* NUMMER DES TITELEINTRAGS UND DER *)
   VERWEIS.DVD[A]:=DISKNR;   (* AKTUELLEN DISKETTENNUMMER EINTRAGEN *)
   IF A = 10 THEN
   BEGIN
     IF VERFILE.FIRSTFREE = -1 THEN
        VERWEIS.FOLGE:= VERFILE.NUMREC
     ELSE
        VERWEIS.FOLGE:= VERFILE.FIRSTFREE;
     PUTREC(VERFILE,RECNRALT,VERWEIS);
     RECNR:= VERWEIS.FOLGE;
     FILLCHAR(VERWEIS,SIZEOF(VERWEIS),0);
     VERWEIS.NUMMER:= RECNR;
     ADDREC(VERFILE,RECNR,VERWEIS);
   END
   ELSE
   PUTREC(VERFILE,RECNRALT,VERWEIS);
END;             (* END PROCEDURE PUTVERWEIS *)
(******************************************)
 (**) (* TRAEGT TITEL IN VERWEIS-DATEI EIN (NEUES STICHWORT) *)
 (**)
PROCEDURE NEWVERWEIS(VAR ANZ : INTEGER);
BEGIN
  CHECKFREEGROUPS;
  IF NOT ENOUGH THEN
     EXIT;
  FILLCHAR(VERWEIS,SIZEOF(VERWEIS),0);
  IF VERFILE.FIRSTFREE = -1 THEN
     DATAF:= VERFILE.NUMREC
  ELSE
     DATAF:= VERFILE.FIRSTFREE;
  VERWEIS.NUMMER:= DATAF;
  VERWEIS.DVR[1]:= ANZ;
  VERWEIS.DVD[1]:= DISKNR;
  ADDREC(VERFILE,DATAF,VERWEIS);
  KEY.KEYCODE:= INTOSTR(DATAF);
  PACK(SCR.S[I],KEY.KEYWORD);
  KEY.KEYSTATUS:=0;
  IF I <= ANZKEY THEN            (* NUR EIGENTLICHE SCHLUESSELWOERTER,   *)
  BEGIN                          (* NICHT AUTOREN ODER JOURNALS WERDEN   *)
     DATREC.Z[I]:= DATAF;         (* IN KEYF U. CODEINDEXFILE AUFGENOMMEN *)
     ADDREC(KEYF,KEYREF,KEY);
     ADDKEY(CODEINDEXFILE,KEYREF,KEY.KEYCODE);
  END;
  ADDKEY(NAMEINDEXFILE,DATAF,KEY.KEYWORD);
  IF OK THEN
     KEY_ANZ:= KEY_ANZ + 1;
  AKTFREEGROUPS;
END;
(*****************************************************************)
(**)     (* AUFNEHMEN EINES NEUEN TITELS *)
(**)
OVERLAY PROCEDURE ADD;
VAR
   AUFGENOMMEN  :  ARRAY[1..ANZGES] OF BOOLEAN;
   PLATZ        :  ARRAY[1..ANZGES] OF INTEGER;
   READY        :  BOOLEAN;
BEGIN
   CHECKFREEGROUPS;
   IF NOT ENOUGH THEN
      EXIT;
   SELECT('ADD: Z)EITSCHRIFT, B)UCH, S)AMMELBAND ?',['Z','B','S',#$0D],
         CH,ART);
   IF CH <> ART THEN
      OUTMASK(CH);
   ART:= CH;
   FILLCHAR(SCR,SIZEOF(SCR),0);
   IF FILELEN(DATF) <> 1 THEN
   BEGIN
      GETREC(DATF,FILELEN(DATF) - 1,DATREC);
      RZAHL:= DATREC.NR;
      IF DATREC.NR < 0 THEN
         RZAHL:= RZAHL + 65536.0;
      IF RZAHL <= 65533.0 THEN
         RZAHL:= RZAHL + 1.0
      ELSE
         RZAHL:= 1.0;

      STR(RZAHL :5:0,SCR.C.NR);
   END
   ELSE
      SCR.C.NR:= '    1';

   OK_1:= TRUE;     (* GIBT AN OB ABSTRACT-VERARBEITUNG OK WAR *)
   REPEAT
     READY:=FALSE;
     WHILE NOT READY DO
     BEGIN
        WITH SCR DO
           INPUTCUST(SCR);

        IF PR.ATRKEY THEN
           MAKE_AUTOR_KEYS;

        IF PR.JRNKEY THEN
           MAKEKEY(SCR.C.ZEITSCHR,SCR.S[ANZGES],2);

        IF (SCR.S[1] = '') AND NOT(PR.ATRKEY AND (SCR.C.AUTOR_A <> ''))
           AND NOT(PR.JRNKEY AND (SCR.C.ZEITSCHR <> '')) THEN
           BEGIN
              IF SCR.C.ABS <> 0 THEN
                 LOESCHE(SCR.C.ABS);
              EXIT;
           END;

        I:=1;
        REPEAT
          READY:= TRUE;
          IF SCR.S[I] <> '' THEN
          BEGIN
             PACK(SCR.S[I],KEY.KEYWORD);
             FINDKEY(NAMEINDEXFILE,DATAF,KEY.KEYWORD);
             PLATZ[I]:=DATAF;
             IF OK THEN
                AUFGENOMMEN[I]:= TRUE   (* STICHWORT BEREITS AUFGENOMMEN *)
             ELSE
             BEGIN          (* STICHWORT NICHT GEFUNDEN *)
                PROMPT:= PRO1;
                IF I > ANZKEY THEN     (* AUTOREN U. ZEITSCHRIFTEN MUESSEN *)
                   INSERT('/S',PROMPT,33);(* NICHT AUFGENOMMEN WERDEN *)
                INSERT(SCR.S[I],PROMPT,12);
                REPEAT
                   SELECT(PROMPT,['Y','N','S',#$0D],CH,'Y');
                   IF CH = 'Y' THEN
                      AUFGENOMMEN[I]:=FALSE;
                   IF CH = 'N'THEN
                      READY:= FALSE;
                   IF CH = 'S' THEN
                      IF I <= ANZKEY THEN
                      BEGIN
                         BEEP;
                         CH:= '$';
                      END
                      ELSE SCR.S[I]:= '';
                UNTIL CH <> '$';
             END;
          END;   (* END IF SCR.S[I] <> '' *)
          I:= I + 1;
        UNTIL NOT READY OR (I = ANZGES + 1);
     END;         (* END WHILE *)
     READY:= FALSE;
     FILLCHAR(DATREC,SIZEOF(DATREC),0);

     ANZ:= FILELEN(DATF);

     STR5_TO_INTEGER(SCR.C.NR,ZAHL,L_OK,TEMPSTR,RZAHL);
     IF L_OK THEN
     BEGIN
       NUM:= INTOSTR(ZAHL);
       NUMDSK:=NUM + CHR(DISKNR);
       FINDKEY(NUMINDEXFILE,DATAF,NUMDSK);
       IF NOT OK THEN
       BEGIN
         READY:=TRUE;
         ADDKEY(NUMINDEXFILE,ANZ,NUMDSK);
       END
       ELSE
       BEGIN
         GOTOXY(1,23);WRITE('DOPPELTE TITEL-NUMMER !');CLREOL;
         READ(KBD,TC);
       END;
     END
     ELSE
     BEGIN
       GOTOXY(1,23);WRITE('FALSCHE TITEL-NUMMER !');CLREOL;
       READ(KBD,TC);
     END;
   UNTIL READY;

   I:=1;
   REPEAT
      IF SCR.S[I] <> '' THEN
      BEGIN
         RECNR:= PLATZ[I];
         IF AUFGENOMMEN[I] THEN
         BEGIN       (* STICHWORT GEFUNDEN *)
            PUTVERWEIS(RECNR,ANZ);
            IF I <= ANZKEY THEN        (* NUR SCHLUESSEL, NICHT AUTOREN *)
                DATREC.Z[I]:=PLATZ[I];  (* ODER JOURNALS *)
         END
         ELSE      (* STICHWORT NICHT GEFUNDEN *)
            NEWVERWEIS(ANZ);   (* STICHWORT NEU AUFNEHMEN *)
      END;
      I:= I + 1;
    UNTIL (I = ANZGES + 1) OR (NOT ENOUGH);
    NEWREC(DATF,REC_ALT);
    FOLGE_1:= 0;
    CONSTRUCT;
    TITEL_ANZ:= TITEL_ANZ + 1;
END;
(*******************************************************)
(**)  (* AENDERN EINES BEREITS VORHANDENEN TITELS *)
(**)
OVERLAY PROCEDURE EDIT;
CONST
  TERM : CHARSET = [#$0D];
VAR
  TC           : CHAR;
  ZUSATZ       : ARRAY[ANZKEY_PLUS_1..ANZGES] OF INTEGER;
  FIRSTREC     : INTEGER;
  L_NUM        : STRING[2];
  TITEL_NR     : INTEGER;
  TITEL_NR_NEU : INTEGER;
  NUMDSK_ALT   : STRING[3];
  NUMDSK_NEU   : STRING[3];
  L_OK         : BOOLEAN;
  DUMMY        : INTEGER;
  L_CH         : CHAR;

(**)
PROCEDURE GETEINTRNR(VAR NUMDSK : STR3;
                     VAR ZAHL   : INTEGER;
                     VAR REC_NR : INTEGER);

VAR
  EINTRAGNR  : STRING[5];
  L_OK       : BOOLEAN;
  L_NUM      : STRING[2];
BEGIN
  REPEAT
    EINTRAGNR:='';
    GOTOXY(1,23);WRITE('TITEL-NR:');CLREOL;
    P:= 0;
    INPUTSTR(EINTRAGNR,5,12,22,TERM,TC,0,P,'.',US_T);
    STR5_TO_INTEGER(EINTRAGNR,ZAHL,L_OK,EINTRAGNR,RZAHL);
  UNTIL L_OK;

  L_NUM:= INTOSTR(ZAHL);
  NUMDSK:= L_NUM + CHR(DISKNR);
  FINDKEY(NUMINDEXFILE,REC_NR,NUMDSK);
  IF NOT OK THEN
  BEGIN
    GOTOXY(1,23);
    WRITE('TITEL-NR. AUF AKT. DATENDISKETTE NICHT VORHANDEN !');
    CLREOL;
    READ(KBD,TC);
    REC_NR:= 0;
  END;
END;

BEGIN       (* PROCEDURE EDIT *)
  CHECKFREEGROUPS;
  IF NOT ENOUGH THEN
     EXIT;

  GETEINTRNR(NUMDSK_ALT,TITEL_NR,EINTRAGINT);
  IF EINTRAGINT = 0 THEN     (* EINTRAGINT = RECORDNUMMER DES TITELS *)
    EXIT;

  IF FILELEN(DATF) > EINTRAGINT THEN
  BEGIN
    GETREC(DATF,EINTRAGINT,DATREC);
    REC_ALT:= EINTRAGINT;
    IF ART <> DATREC.ART THEN
       OUTMASK(DATREC.ART);
    RECONSTRUCT(SCR);
  END
  ELSE
  BEGIN
    GOTOXY(1,23);WRITE('FALSCHE EINTRAG-NR.!');CLREOL;
    READ(KBD,CH);
    EXIT;
  END;
  (**)
  (**)
  FOR I:= 1 TO ANZKEY DO     (* STICHWOERTER UEBER CODEINDEXFILE *)
  BEGIN                  (* AUS KEYFILE HOLEN UND IN SCR.S EINTRAGEN *)
    IF DATREC.Z[I] <> 0 THEN
    BEGIN
      KEY.KEYCODE:= INTOSTR(DATREC.Z[I]);
      FINDKEY(CODEINDEXFILE,DATAF,KEY.KEYCODE);
      IF OK THEN
      BEGIN
        GETREC(KEYF,DATAF,KEY);
        UNPACK(KEY.KEYWORD,SCR.S[I]);
      END;       (* END IF OK THEN *)
    END;         (* END IF DATREC.Z[I] <> 0 THEN *)
  END;           (* END FOR I:=1 TO ANZKEY *)
  (**)
  (**)
  WITH SCR DO
  BEGIN
    OUTSCR(SCR);      (* AN BILDSCHIRM AUSGEBEN *)

    MAKE_AUTOR_KEYS;
    MAKEKEY(SCR.C.ZEITSCHR,SCR.S[ANZGES],2);
    ALTKEY:= SCR.S;     (* ALTE STICHWOERTER IN ALTKEY SICHERN *)

    (**)
    OK_1:= TRUE;         (* FLAG FUER ABSTRACT-VERARBEITUNG *)
    REPEAT     (* UNTIL REPCOND = FALSE *)
      REPCOND:= FALSE;
      INPUTCUST(SCR);     (* VERAENDERUNGEN EINLESEN *)
      FOR I:= 1 TO (ANZATR + 1) DO
         SCR.S[I + ANZKEY]:= '';

      IF PR.ATRKEY THEN
         MAKE_AUTOR_KEYS;
      IF PR.JRNKEY THEN
         MAKEKEY(SCR.C.ZEITSCHR,SCR.S[ANZGES],2);

      FOR I:= 1 TO ANZGES DO   (* FUER JEDES STICHWORT IN SCR.S WIRD *)
      BEGIN                    (* GEPRUEFT, OB ES SCHON VORHER VORHANDEN *)
        A:= 1;                 (* WAR. ENTSPRECHEND WIRD DAS FLAG (INKEY) *)
        REPEAT                 (* AUF 0 ODER 1 GESETZT *)
          IF (SCR.S[I] = ALTKEY[A]) OR (SCR.S[I] = '') THEN
          BEGIN
            INKEY[I]:= 0; (* ALTES STICHWORT *)
            A:= ANZGES + 1;       (* ZUM SCHLEIFENABBRUCH *)
          END
          ELSE
          BEGIN
            INKEY[I]:= 1;  (* NEUES STICHWORT, MUSS AUFGENOMMEN *)
            A:= A + 1;     (* WERDEN *)
          END;
        UNTIL A = (ANZGES + 1);
      END;                (* END FOR I:= 1 TO ANZGES *)
      (**)
      (**)
      I:= 1;                     (* FESTSTELLEN, OB ES SICH UM SCHON *)
      REPEAT                     (* BEKANNTE STICHWORTE HANDELT *)
        IF SCR.S[I] <> '' THEN
        BEGIN
          PACK(SCR.S[I],KEY.KEYWORD);
          FINDKEY(NAMEINDEXFILE,DATAF,KEY.KEYWORD);
          IF OK THEN
          BEGIN
            IF I <= ANZKEY THEN
               DATREC.Z[I]:= DATAF
            ELSE
               ZUSATZ[I]:= DATAF;
            I:= I + 1;
          END
          ELSE
          BEGIN                 (* NEUES STICHWORT *)
            PROMPT:= PRO1;
            IF I > ANZKEY THEN
               INSERT('/S',PROMPT,33);
            INSERT(SCR.S[I],PROMPT,12);
            REPEAT
               SELECT(PROMPT,['Y','N','S',#$0D],CH,'Y');
               IF CH = 'Y' THEN
               BEGIN
                 INKEY[I]:= 2;   (* NOCH NICHT IN LISTE VORHANDEN *)
                 I:= I + 1;
               END;
               IF CH = 'N' THEN
               BEGIN              (* STICHWORT SOLL NICHT AUFGENOMMEN WERDEN *)
                  REPCOND:= TRUE;
                  I:= ANZGES + 1;      (* ZUM ABBRUCH DER SCHLEIFE *)
               END;
               IF CH = 'S' THEN
                  IF I <= ANZKEY THEN
                  BEGIN
                     BEEP;
                     CH:= '$';
                  END
                  ELSE INKEY[I]:= 3; (* AUTOR ODER ZEITSCHRIFT SOLL NICHT *)
            UNTIL CH <> '$';         (* AUFGENOMMEN WERDEN *)
          END;
        END
        ELSE
        BEGIN            (* SCR.S[I] = '' *)
          IF I <= ANZKEY THEN
             DATREC.Z[I]:= 0
          ELSE
             ZUSATZ[I]:= 0;
          I:= I + 1;
        END;
      UNTIL I = (ANZGES + 1);
      STR5_TO_INTEGER(C.NR,TITEL_NR_NEU,L_OK,TEMPSTR,RZAHL);
      L_NUM:=INTOSTR(TITEL_NR_NEU);
      NUMDSK_NEU:= L_NUM + CHAR(DISKNR);
      IF NUMDSK_ALT <> NUMDSK_NEU THEN    (* NEUE TITEL-NUMMER *)
      BEGIN
         IF L_OK THEN
         BEGIN
            DELETEKEY(NUMINDEXFILE,DUMMY,NUMDSK_ALT);
            ADDKEY(NUMINDEXFILE,DUMMY,NUMDSK_NEU);
         END
         ELSE
            OK:= FALSE;
         IF NOT OK THEN
         BEGIN
            ADDKEY(NUMINDEXFILE,DUMMY,NUMDSK_ALT);
            GOTOXY(1,23);
            WRITE('NEUE TITEL-NUMMER FALSCH ! ');
            WRITE('ALTE TITEL WAR : ',INT_TO_STR5(TITEL_NR));CLREOL;
            READ(KBD,L_CH);
            REPCOND:= TRUE;
         END
         ELSE NUMDSK:= NUMDSK_NEU;
      END;


    UNTIL REPCOND = FALSE;
  END;                    (* END - WITH SCR DO *)

  FOR I:= 1 TO ANZGES DO      (* EINTRAGEN IN VERWEIS DATEI *)
  BEGIN
    CASE INKEY[I] OF
    0 : BEGIN             (* TITEL IST BEREITS UNTER DIESEM STICH- *)
        END;              (* WORT GEFUEHRT *)
    1 : BEGIN                (* TITEL SOLL UNTER ZUSAETZLICHEM, ABER  *)
          IF I <= ANZKEY THEN
            RECNR:= DATREC.Z[I]  (* SCHON BEKANNTEM STICHWORT GEFUEHRT WERDEN *)
          ELSE
            RECNR:= ZUSATZ[I];
          ANZ:=EINTRAGINT;
          PUTVERWEIS(RECNR,ANZ);
        END;
    2 : BEGIN   (* TITEL SOLL UNTER NOCH UNBEKANNTEM STICHWORT GEFUEHRT *)
          ANZ:=EINTRAGINT;  (* WERDEN *)
          NEWVERWEIS(ANZ);
          IF NOT ENOUGH THEN    (* KEIN PLATZ ZUM AUFNEHMEN NEUER SCLUESSEL *)
          BEGIN
             A:= I;
             REPEAT
                IF INKEY[A] = 2 THEN
                BEGIN
                   IF A <= ANZKEY THEN
                   BEGIN
                      INKEY[A]:= 0;
                      DATREC.Z[A]:= 0;
                   END
                   ELSE INKEY[A]:= 3;
                END;
                A:= A + 1;
             UNTIL A = (ANZGES + 1);
          END;  (* END IF NOT ENOUGH *)
        END;    (* END 2: *)
    3 : BEGIN   (* AUTOR ODER ZEITSCHRIFT NICHT AUFNEHMEN *)
        END;
      END;      (* END CASE *)
  END;        (* END FOR I:= 1 TO ANZGES *)

  ANZ:=EINTRAGINT;
  CONSTRUCT;         (* TITEL MIT NEUEN STICHWORTEN ABSPEICHERN *)
  (**)

  FOR I:= 1 TO ANZGES DO   (* NICHT MEHR BENUTZTE STICHWORTE IN VERWEIS- *)
  BEGIN                    (* DATEI LOESCHEN *)
    A:= 1;
    REPEAT
      IF (ALTKEY[I] = SCR.S[A]) AND (INKEY[I] <> 3) THEN
      BEGIN
        ALTKEY[I]:= '';  (* NOCH RICHTIGE STICHWORTE AUSSCHLIESSEN *)
        A:= ANZGES + 1;          (* SCHLEIFENABBRUCH BEI UEBEREINSTIMMUNG *)
      END
      ELSE
      A:= A + 1;
    UNTIL A = ANZGES + 1;
  END;
  FOR I:= 1 TO ANZGES DO
  BEGIN
    IF ALTKEY[I] <> '' THEN
    BEGIN
      PACK(ALTKEY[I],KEY.KEYWORD);
      FINDKEY(NAMEINDEXFILE,RECNR,KEY.KEYWORD);
      IF OK THEN
      BEGIN
        REPCOND:= TRUE;
        FIRSTREC:= RECNR;
        REPEAT
          GETREC(VERFILE,RECNR,VERWEIS);
          RECNRALT:= RECNR;
          RECNR:= VERWEIS.FOLGE;
          A:= 1;
          REPEAT
            IF (VERWEIS.DVR[A] = EINTRAGINT) AND
               (VERWEIS.DVD[A] = DISKNR) THEN
            BEGIN
               REPEAT
                  WHILE A < ANZVER DO
                  BEGIN
                     VERWEIS.DVR[A]:= VERWEIS.DVR[A + 1];
                     VERWEIS.DVD[A]:= VERWEIS.DVD[A + 1];
                     A:= A + 1;
                  END;   (* END WHILE *)
                  IF VERWEIS.DVR[A - 1] <> 0 THEN
                  BEGIN
                     GETREC(VERFILE,VERWEIS.FOLGE,VERWEIS1);
                     IF VERWEIS1.DVR[1] <> 0 THEN
                     BEGIN
                        VERWEIS.DVR[A]:= VERWEIS1.DVR[1];
                        VERWEIS.DVD[A]:= VERWEIS1.DVD[1];
                        PUTREC(VERFILE,RECNRALT,VERWEIS);
                        RECNRALT:= VERWEIS.FOLGE;
                        VERWEIS:= VERWEIS1;
                        A:= 1;
                     END
                     ELSE    (* ELSE IF VERWEIS.DVR[1] <> 0 THEN *)
                     BEGIN
                        DELETEREC(VERFILE,VERWEIS.FOLGE);
                        VERWEIS.DVR[ANZVER]:= 0;
                        VERWEIS.DVD[ANZVER]:= 0;
                        VERWEIS.FOLGE:= 0;
                        PUTREC(VERFILE,RECNRALT,VERWEIS);
                        REPCOND:= FALSE;
                        A:= ANZVER + 1;
                     END;     (* END IF VERWEIS.DVR[1] <> 0 THEN *)
                  END
                  ELSE    (* ELSE IF VERWEIS.DVR[A - 1] <> 0 THEN *)
                  BEGIN
                     IF (RECNRALT = FIRSTREC) AND (VERWEIS.DVR[1] = 0) THEN
                     BEGIN
                        DELETEREC(VERFILE,RECNRALT);
                        DELETEKEY(NAMEINDEXFILE,FIRSTREC,KEY.KEYWORD);
                        IF OK THEN
                           KEY_ANZ:= KEY_ANZ - 1;
                        IF I <= ANZKEY THEN
                        BEGIN
                           KEY.KEYCODE:= INTOSTR(RECNRALT);
                           DELETEKEY(CODEINDEXFILE,KEYREF,KEY.KEYCODE);
                           DELETEREC(KEYF,KEYREF);
                        END;
                     END
                     ELSE
                     PUTREC(VERFILE,RECNRALT,VERWEIS);

                     REPCOND:= FALSE;
                     A:= ANZVER + 1;
                  END;   (* END IF VERWEIS.DVR[ A -1] <> 0 THEN *)
               UNTIL (REPCOND = FALSE);
            END
            ELSE    (* ELSE IF (VERWEIS.DVR[A] = EINTRAGINT) .... *)
            A:= A + 1;
          UNTIL A = ANZVER + 1;
        UNTIL (REPCOND = FALSE) OR (RECNR = 0);
      END;   (* END IF OK *)
    END;     (* END IF ALTKEY[I] <> '' *)
  END;       (* END FOR I:= 1 TO ANZGES *)
END;         (* END PROCEDURE EDIT *)

   (**)
   (**)
BEGIN      (* UPDATE *)
   ART:= 'Z';
   OUTMASK(ART);
   REPEAT
      SELECT('UPDATE: A)DD, E)DIT, Q)UIT ?',['A','E','Q',#$0D],CH,'A');
      CASE CH OF
        'A' : ADD;
        'E' : EDIT;
      END;
      IF CH <> 'Q' THEN
      BEGIN
        GOTOXY(50,2);WRITE(INT_TO_STR5(TITEL_ANZ));
        GOTOXY(64,2);WRITE(INT_TO_STR5(KEY_ANZ));
        AKTFREEGROUPS;
        Z25;
        CLEARFORM;
      END;
   UNTIL CH = 'Q';
END;  (* END PROCEDURE UPDATE *)