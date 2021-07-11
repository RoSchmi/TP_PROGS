(**********************************************************************)
(*                                                                    *)
(*       INSTALLATIONSPROGRAMM FUER LITERATURARCHIVIERUNGSSYSTEM      *)
(*                                                                    *)
(**********************************************************************)
PROGRAM LITINST;
(*$A+,C-,R-,V-*)
CONST
MAXDATARECSIZE = 256;
MAXKEYLEN      = 26;
PAGESIZE       = 16;
ORDER          = 8;
PAGESTACKSIZE  = 7;
MAXHEIGHT      = 7;
(*$IACCESS3.BOX*)
TYPE
   RECORDBUFF = ARRAY[1..256] OF BYTE;
   CHARSET    = SET OF CHAR;
   STR12      = STRING[12];
   ANYSTR     = STRING[255];
   STR238     = STRING[238];
   STR80      = STRING[80];
   STR30      = STRING[30];
   PARMRECORD = RECORD
      FORMFEED         : CHAR;
      ATRKEY           : BOOLEAN;
      JRNKEY           : BOOLEAN;
      USTAST           : BOOLEAN;
      IBM_DRUCKER      : BOOLEAN;
      DRUCKINITSTR     : STRING[20];
      US_UMSCHALT      : STRING[20];
      GER_UMSCHALT     : STRING[20];
      BEEP             : BOOLEAN;
      IDXPFAD          : STR30;
      TITPFAD          : STR30;
   END;
VAR
   BUFFER       : RECORDBUFF;
   CH           : CHAR;
   PR           : PARMRECORD;
   P,I          : INTEGER;
   PARMF        : DATAFILE;
TYPE
      PARAM = RECORD
         EINRUECK     : INTEGER;
         SEITENLAENGE : INTEGER;
         OBERER_RAND  : INTEGER;
         ZEILENZAHL   : INTEGER;
         LINKER_RAND  : INTEGER;
         RECHTER_RAND : INTEGER;
         DRUCKSTOP    : BOOLEAN;
         FORMFEED     : BOOLEAN;
      END;
   FORMAT_BUFFER = RECORD
      FORM_TEXT : ARRAY[1..238] OF BYTE;
      P         : PARAM;
   END;
VAR
   FORM_BUF : FORMAT_BUFFER ABSOLUTE BUFFER;
   FORM_STR : STR238;
CONST
   Z_INIT_1_A = '<NR> <TITEL> <AUTOR> <ZEITSCHR> BAND: <BAND> (<JAHR>)';
   Z_INIT_1_B = ' SEITE: <SEITE>';
   Z_INIT_1_C = '';

   B_INIT_1_A = '<NR> <TITEL> <AUTOR> <BVERLAG> <BORT> (<JAHR>)';
   B_INIT_1_B = ' SEITE: <SEITE>';
   B_INIT_1_C = '';

   S_INIT_1_A = '<NR> <TITEL> <AUTOR> in: <HRSGTITEL>, Hrsg.: ';
   S_INIT_1_B = '<HRSG>, Verlag: <SVERLAG>, <SORT> (<JAHR>)';
   S_INIT_1_C = ' SEITE: <SEITE>';

   PARAM_INIT : PARAM = (EINRUECK     : 5;
                         SEITENLAENGE : 72;
                         OBERER_RAND  : 3;
                         ZEILENZAHL   : 66;
                         LINKER_RAND  : 6;
                         RECHTER_RAND : 85;
                         DRUCKSTOP    : TRUE;
                         FORMFEED     : FALSE);

   PR_INIT : PARMRECORD =
             (FORMFEED:#12;
             ATRKEY: FALSE;
             JRNKEY: FALSE;
             USTAST: FALSE;
             IBM_DRUCKER: FALSE;
             DRUCKINITSTR:'';
             US_UMSCHALT:'';
             GER_UMSCHALT:'';
             BEEP: FALSE;
             IDXPFAD: 'LIT';
             TITPFAD: 'LIT01');
(************************************************)
FUNCTION CONSTR(C: CHAR; N: INTEGER) : STR80;
VAR
   S : STRING[80];
BEGIN
  IF N < 0 THEN
  N:=0;
  S[0]:=CHR(N);
  FILLCHAR(S[1],N,C);
  CONSTR:= S;
END;

PROCEDURE BEEP;
BEGIN
END;

(*******************************************************)
PROCEDURE INPUTSTR(VAR      S      : ANYSTR;
                            L,X,Y  : INTEGER;
                            TERM   : CHARSET;
                   VAR      TC     : CHAR;
                            K      : INTEGER;
                   VAR      P      : INTEGER);
VAR
   CH,TA      : CHAR;
   FUNCKEY    : BOOLEAN;
   ESZET      : BOOLEAN;
   UNDERSCORE : CHAR;
BEGIN
   IF P = -1 THEN
      P:= 0;

   IF K = 6 THEN         (* FUER ABSTRACT DATEI ANDERES UNDERSCORE *)
      UNDERSCORE:= ' '
   ELSE
      UNDERSCORE:= '.';

   GOTOXY(X + 1,Y + 1);WRITE(S,CONSTR(UNDERSCORE,L - LENGTH(S)));
   ESZET:= FALSE;
   REPEAT
     GOTOXY(X+P+1,Y+1);
     FUNCKEY:=FALSE;
     IF ESZET THEN
     TA:= 'S'
     ELSE READ(KBD,TA);

     IF PR.USTAST THEN     (* UMFORMUNG IN DEUTSCHE UMLAUTE *)
     BEGIN
        CASE TA OF
        '|': TA:= #148;
        '\': TA:= #153;
        '{': TA:= #132;
        '[': TA:= #142;
        '}': TA:= #129;
        ']': TA:= #154;
        '~': TA:= #225;
        END;
     END;
     IF (TA = #27) AND KEYPRESSED THEN   (* IBM-PC SONDERTASTEN ? *)
     BEGIN
       READ(KBD,TA);
       FUNCKEY:=TRUE;
     END;
     IF FUNCKEY = FALSE THEN
     CH:=TA
     ELSE
     CASE TA OF
       #75  :  CH:=#$13;      (* LINKER PFEIL        = CTRL-S *)
       #77  :  CH:=#$04;      (* RECHTER PFEIL       = CTRL-D *)
       #115 :  CH:=#$01;      (* CTRL-LINKER PFEIL   = CTRL-A *)
       #116 :  CH:=#$06;      (* CTRL-RECHTER PFEIL  = CTRL-F *)
       #83  :  CH:=#$07;      (* DEL                 = CTRL-G *)
       #72  :  CH:=#$05;      (* PFEIL HOCH          = CTRL-E *)
       #80  :  CH:=#$18;      (* PFEIL RUNTER        = CTRL-X *)
       #79  :  CH:=#$1A;      (* END                 = CTRL-Z *)
       #73  :  CH:=#$12;      (* PAGE UP             = CTRL-R *)
       #132 :  CH:=#$14;      (* CTRL-PAGE UP        = CTRL-T *)
       #81  :  CH:=#$03;      (* PAGE DOWN           = CTRL-C *)
       #118 :  CH:=#$16;      (* CTRL-PAGE DOWN      = CTRL-V *)
       #59  :  CH:=#$0C;      (* F1                  = CTRL-L *)
       ELSE CH:=#00;
     END;


     IF (K = 1) OR (K = 3) THEN
     BEGIN
        ESZET:= FALSE;
        CH:= UPCASE(CH);
        CASE CH OF
        #$00..#$5F,#153,#142,#154 : BEGIN END;
        #148                      : CH:= #153;
        #132                      : CH:= #142;
        #129                      : CH:= #154;
        #225                      : BEGIN
                                      CH:= 'S';
                                      ESZET:= TRUE;
                                    END;
        ELSE CH:= #00;
        END;
     END;

     IF (K = 3) THEN     (* ' ','#'UND '$' BEI LIST NICHT ERLAUBT *)
        IF (CH IN [' ','#'..'$']) AND (P = 0) THEN
           CH:= #00;

     IF K = 2 THEN   (* NUR ZAHLEN VON 0 BIS 9 ZUGELASSEN *)
        IF NOT(CH IN [#$01,#$04,#$05,#$06,#$07,#$0C,#$0D,#$13,#$18,#$19,#$1A,
        ' ','0'..'9']) THEN
           CH:= #00;

     IF K = 4 THEN   (* NUR HEX-ZAHLEN, $ UND BLANK ZUGELASSEN *)
       IF NOT (CH IN [#$05,#$04,#$08,#$0D,#13,#$24,' ','0'..'9','A'..'F']) THEN
           CH:= #00;

     IF K = 5 THEN               (* DIE ZEICHEN '>','=' UND '>' SIND ALS *)
        IF CH IN ['<'..'>'] THEN (* FUER AUTOREN U. QUELLEN NICHT ERLAUBT *)
           CH:= #00;

     CASE CH OF
     #32..#126,#129,#132,#142,#148,#153,#154,#225
                  :   IF P < L THEN
                        BEGIN
                          IF LENGTH(S) = L THEN
                             DELETE(S,L,1);
                          P:=P + 1;
                          IF P > (LENGTH(S) + 1) THEN
                          BEGIN
                            FILLCHAR(S[LENGTH(S) + 1],P - LENGTH(S) - 1,#$20);
                            S[0]:= CHR(P - 1);
                          END;
                          INSERT(CH,S,P);
                          WRITE(COPY(S,P,L));
                        END
                        ELSE BEEP;
       #$13       :   IF P > 0 THEN              (* CTRL-S = CHAR LEFT *)
                         P:=P - 1
                         ELSE BEEP;
       #$04       :   IF P < L THEN              (* CTRL-D = CHAR RIGHT *)
                         P:=P + 1
                         ELSE BEEP;
       #$01       :   P:= 0;                     (* CTRL-A = WORD LEFT *)
       #$06       :   P:=LENGTH(S);              (* CTRL-F = WORD RIGHT *)
       #$07       :   IF P < LENGTH(S) THEN (* CTRL-G = DEL CHAR UND. CURS. *)
                      BEGIN
                        DELETE(S,P + 1,1);
                        WRITE(COPY(S,P + 1,L),UNDERSCORE);
                      END;
       #$08       :   IF P > 0 THEN              (* CTRL-H = DEL LEFT CHAR. *)
                      BEGIN
                        DELETE(S,P,1);
                        WRITE(#$08,COPY(S,P,L),UNDERSCORE);
                        P:= P - 1;
                      END
                      ELSE
                         BEEP;
       #$19       :   BEGIN                      (* CTRL-Y = DEL LINE *)
                        WRITE(CONSTR(UNDERSCORE,LENGTH(S) - P));
                        DELETE(S,P + 1,L);
                      END;
   ELSE
     IF NOT (CH IN TERM) THEN BEEP;
   END; (* OF CASE *)
  UNTIL CH IN TERM;
  K:= LENGTH(S);
  GOTOXY(X + K + 1,Y + 1);
  WRITE('':L - K);
  TC:= CH;
END;
PROCEDURE SELECT(    PROMPT   :   STR80;
                     TERM     :   CHARSET;
                 VAR TC       :   CHAR;
                     PRESET   :   CHAR);
VAR
   TA,CH   : CHAR;
   FUNCKEY : BOOLEAN;
BEGIN
GOTOXY(1,23); WRITE(PROMPT,' ',PRESET);CLREOL;
GOTOXY(LENGTH(PROMPT) + 2,23);
REPEAT
   READ(KBD,TA);
   IF (TA = #27) AND KEYPRESSED THEN (* IBM-PC SONDERTASTE ? *)
   BEGIN
      READ(KBD,TA);
      FUNCKEY:= TRUE;
   END
   ELSE FUNCKEY:= FALSE;
   IF NOT FUNCKEY THEN
      CH:= UPCASE(TA)
   ELSE
   CASE TA OF
      #72  : CH:= #$05;     (* PFEIL HOCH          = CTRL-E *)
      ELSE CH:= #00;
   END;

   TC:= CH;
   IF NOT (TC IN TERM) THEN
      BEEP;
   UNTIL TC IN TERM;
   IF TC = #$0D THEN
   BEGIN
     TC:= PRESET;CH:= PRESET;
   END;
   IF TC IN ['Y','N'] THEN
      WRITE(CH);
END;

PROCEDURE CONV_HEX(    INSTR  : STR80;
                   VAR OUTSTR : ANYSTR);
VAR
   I,A  : INTEGER;
BEGIN
   IF ORD(INSTR[0]) > 15 THEN
      INSTR[0]:= CHR(15);
   IF INSTR[0] > #00 THEN
   BEGIN
      FOR I:= 1 TO LENGTH(INSTR) DO
      BEGIN
         OUTSTR[ (I-1) * 4 + 1]:= '$';
         A:= ORD(INSTR[I]) DIV 16;
         IF (A >= 0) AND (A <= 9) THEN
            A:= A + $30
         ELSE A:= A + $37;
         OUTSTR[ (I-1) * 4 + 2]:= CHR(A);
         A:= ORD(INSTR[I]) MOD 16;
         IF (A >= 0) AND (A <= 9) THEN
            A:= A + $30
         ELSE A:= A + $37;
         OUTSTR[ (I-1) * 4 + 3]:= CHR(A);
         OUTSTR[ (I-1) * 4 + 4]:= ' ';
      END;
      OUTSTR[0]:= CHR(LENGTH(INSTR) * 4 - 1);
   END
   ELSE OUTSTR[0]:= #00;
END;

FUNCTION DEZIMAL(CH : CHAR) : INTEGER;
VAR
   N : INTEGER;
BEGIN
   IF CH IN ['0'..'9'] THEN
      N:= ORD(CH) - 48
   ELSE
      IF CH IN ['A'..'F'] THEN
         N:= ORD(CH) - 55
      ELSE
         N:= 0;
   DEZIMAL:= N;
END;

PROCEDURE CONV_STR(    STR1 : STR80;
                   VAR STR2 : STR80);
VAR
   I  : INTEGER;
   CH : CHAR;
BEGIN
   FOR I:= 1 TO LENGTH(STR1) DO
      IF STR1[I] = ' ' THEN DELETE(STR1,I,1);
   STR2:= '';
   REPEAT
      I:= POS('$',STR1);
      IF I <> 0 THEN
      BEGIN
         CH:= CHR((DEZIMAL(STR1[I + 1]) * 16) + (DEZIMAL(STR1[I + 2])));
         DELETE(STR1,I,1);
         STR2:= STR2 + CH;
      END;
   UNTIL I = 0;
END;

PROCEDURE INPUTPARM(VAR PR : PARMRECORD);
CONST
   TERM : CHARSET = [#$0D,#$05];
VAR
   L      :  INTEGER;
   CH,
   TC,CR  : CHAR;
   BUFSTR : STRING[60];
BEGIN
   L:= 1;
   GOTOXY(1,25);
   WRITE('EINGABE: WEITER ("RETURN")   ZURUECK ("PFEIL HOCH" ODER CTLR.-E.)');
   WITH PR DO
   REPEAT
      P:= 0;
      GOTOXY(1,23);
      CLREOL;
      CASE L OF
  1: BEGIN
        IF ATRKEY THEN
           CR:= 'Y'
        ELSE CR:= 'N';
        SELECT('AUFNAHME DER AUTOREN ALS SCHLUESSEL (Y/N)?',['Y','N',#$05,
               #$0D],CH,CR);
        IF CH = 'Y' THEN ATRKEY:= TRUE;
        IF CH = 'N' THEN ATRKEY:= FALSE;
        IF CH IN ['Y','N'] THEN
           TC:= #$0D
        ELSE TC:= #$05;
     END;
  2: BEGIN
        IF JRNKEY THEN
           CR:= 'Y'
        ELSE CR:= 'N';
        SELECT('AUFNAHME DER QUELLEN ALS SCHLUESSEL (Y/N)?',['Y','N',#$0D,
                #$05],CH,CR);
        IF CH = 'Y' THEN JRNKEY:= TRUE;
        IF CH = 'N' THEN JRNKEY:= FALSE;
        IF CH IN ['Y','N'] THEN
           TC:= #$0D
        ELSE TC:= #$05;
     END;
  3: BEGIN
        IF USTAST THEN
           CR:= 'Y'
        ELSE CR:= 'N';
        SELECT('US-TASTATUR (Y/N)?',['Y','N',#$05,#$0D],CH,CR);
        IF CH = 'Y' THEN USTAST:= TRUE;
        IF CH = 'N' THEN USTAST:= FALSE;
        IF CH IN ['Y','N'] THEN
           TC:= #$0D
        ELSE TC:= #$05;
     END;
  4:
     BEGIN
        CONV_HEX(DRUCKINITSTR,BUFSTR);
        GOTOXY(1,23);
        WRITE('DRUCKER-INIT-STR:');CLREOL;
        INPUTSTR(BUFSTR,60,18,22,TERM,TC,4,P);
        CONV_STR(BUFSTR,DRUCKINITSTR);
     END;
  5: BEGIN
        IF IBM_DRUCKER THEN
           CR:= 'Y'
        ELSE CR:= 'N';
        SELECT('IBM KOMPATIBLER DRUCKER (Y/N)?',['Y','N',#$0D,#$05],CH,CR);
        IF CH = 'Y' THEN IBM_DRUCKER:= TRUE;
        IF CH = 'N' THEN IBM_DRUCKER:= FALSE;
        IF CH IN ['Y','N'] THEN
           TC:= #$0D
        ELSE TC:= #$05;
     END;
  6: BEGIN
        GOTOXY(1,23);
        WRITE('US-ZEICHENSATZ:');CLREOL;
        CONV_HEX(US_UMSCHALT,BUFSTR);
        INPUTSTR(BUFSTR,60,16,22,TERM,TC,4,P);
        CONV_STR(BUFSTR,US_UMSCHALT);
     END;
  7: BEGIN
        GOTOXY(1,23);
        WRITE('GERM.-ZEICHENSATZ:');CLREOL;
        CONV_HEX(GER_UMSCHALT,BUFSTR);
        INPUTSTR(BUFSTR,60,19,22,TERM,TC,4,P);
        CONV_STR(BUFSTR,GER_UMSCHALT);
     END;
  8: BEGIN
        REPEAT
           GOTOXY(1,23);
           WRITE('FORMFEED-CHARACTER ($00-$FF),($31 = AUTOSTOP):');CLREOL;
           BUFSTR[1]:= FORMFEED;
           BUFSTR[0]:= CHR(1);
           CONV_HEX(BUFSTR,BUFSTR);
           INPUTSTR(BUFSTR,3,47,22,TERM,TC,4,P);
           CONV_STR(BUFSTR,BUFSTR);
           FORMFEED:= BUFSTR[1];
        UNTIL FORMFEED IN [#$00..#$FF];
     END;
  9: BEGIN
        IF BEEP THEN
           CR:= 'Y'
        ELSE
           CR:= 'N';
        SELECT('IBM-KOMPATIBLER BEEP (Y/N)?',['Y','N',#$0D,#$05],CH,CR);
        IF CH = 'Y' THEN BEEP:= TRUE;
        IF CH = 'N' THEN BEEP:= FALSE;
        IF CH IN ['Y','N'] THEN
           TC:= #$0D
        ELSE
           TC:= #$05;
     END;
 END;      (* END CASE *)
 IF TC = #$0D THEN
    IF L = 9 THEN
       L:= 1
    ELSE
       L:= L + 1;
 IF TC = #$05 THEN
    IF L = 1 THEN
       L:= 9
    ELSE
       L:= L - 1;
   UNTIL (TC = #$0D) AND (L = 1);
END;

PROCEDURE CLEARBUFFER;
BEGIN
   FOR I:= 1 TO SIZEOF(BUFFER) DO
      BUFFER[I]:= 0;
END;

BEGIN           (* BEGIN MAIN PROGRAM *)
   CLRSCR;
   GOTOXY(10,12);
   WRITE('INSTALLIERUNGSPROGRAMM FURE LITERATURARCHIVIERUNGSSYSTEM');
   GOTOXY(1,22);
   WRITE(CONSTR('-',79));
   GOTOXY(1,24);
   WRITE(CONSTR('-',79));
   GOTOXY(1,25);WRITE('BITTE TASTE DRUECKEN!');
   READ(KBD,CH);
   GOTOXY(1,25);CLREOL;
   OPENFILE(PARMF,'LIPARM.PRM',256);
   IF NOT OK THEN
   BEGIN
 SELECT('PARAMETER-DATEI "LIPARM.PRM" FEHLT. CREATE (Y/N)?',['Y','N'],CH,' ');
    IF CH = 'Y' THEN
    BEGIN
             (* PLATZ FUER ALLGEMEINE PARAMETER RESERVIEREN *)
       MAKEFILE(PARMF,'LIPARM.PRM',256);
       ADDREC(PARMF,I,BUFFER);

             (* INITIALISIERUNG FUER AUSDRUCK VON ZEITSCHRIFTEN *)
       CLEARBUFFER;
       FORM_STR:= Z_INIT_1_A + Z_INIT_1_B + Z_INIT_1_C + #$1A;
       MOVE(FORM_STR[1],FORM_BUF.FORM_TEXT,LENGTH(FORM_STR));
       FORM_BUF.P:= PARAM_INIT;
       ADDREC(PARMF,I,BUFFER);

             (* INITIALISIERUNG FUER AUSDRUCK VON BUECHERN *)
       CLEARBUFFER;
       FORM_STR:= B_INIT_1_A + B_INIT_1_B + B_INIT_1_C + #$1A;
       MOVE(FORM_STR[1],FORM_BUF.FORM_TEXT,LENGTH(FORM_STR));
       FORM_BUF.P:= PARAM_INIT;
       ADDREC(PARMF,I,BUFFER);

             (* INITIALISIERUNG FUER AUSDRUCK VON SAMMELBAENDEN *)
       CLEARBUFFER;
       FORM_STR:= S_INIT_1_A + S_INIT_1_B + S_INIT_1_C + #$1A;
       MOVE(FORM_STR[1],FORM_BUF.FORM_TEXT,LENGTH(FORM_STR));
       FORM_BUF.P:= PARAM_INIT;
       ADDREC(PARMF,I,BUFFER);

             (* VORBESETZEN DER ALLGEMEINEN PARAMETER MIT STANDARDWERTEN *)
       PR:= PR_INIT;
    END
    ELSE EXIT;
   END
   ELSE
   BEGIN
      GETREC(PARMF,1,BUFFER);
      MOVE(BUFFER,PR,SIZEOF(PR));
   END;
   REPEAT
      INPUTPARM(PR);
      GOTOXY(1,25);CLREOL;
      SELECT('INITIALISIERUNG BEENDET. ANNEHMEN (Y/N)?',['Y','N'],CH,' ');
   UNTIL CH = 'Y';
   CLRSCR;
   GOTOXY(1,23);
   WRITE('INITIALISIERUNG WIRD IN DATEI "LIPARM.PRM" UEBERTRAGEN.');
   CLEARBUFFER;
   MOVE(PR,BUFFER,SIZEOF(PR));
   PUTREC(PARMF,1,BUFFER);
   CLOSEFILE(PARMF);
   CLRSCR;
   END.

