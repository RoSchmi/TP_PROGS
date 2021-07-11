OVERLAY PROCEDURE SUCHE;
TYPE
   STR11 = STRING[11];
   ITEM_ARRAY = ARRAY[1..11] OF STR11;
CONST
   ITEM_NR : ITEM_ARRAY = ('<SORT-FELD>','<NR>','<TITEL>','<AUTOR>','<JAHR>',
                           '<SEITE>','<ABSTR>','','','','');

VAR
   RZAHL                      : REAL;
   TC,CH                      : CHAR;
   KW1,KW2,KWINT              : KEYNAMSTR;
   OPERATOR                   : STRING[1];
   TEXTDATEI                  : STR30;
   PFAD                       : STRING[35];
   DATLIST                    : DATEILISTE;
   RECONR                     : ARRAY[1..16] OF INTEGER;
   BLOCK                      : INTEGER;
   HEAPTOP                    : ^INTEGER;
   HEAP_RELEASED              : BOOLEAN;
   PRETIT2                    : TITPTR;
   MATCH                      : BOOLEAN;
   C,I,L,RECNR1,RECNR2        : INTEGER;
   NEUEDISK                   : BOOLEAN;
   ALTIDXPFAD                 : STR30;
   DR_STR                     : STRING[5];
   UNTERER_RAND               : INTEGER;
   PROMPT                     : STR80;
   (**)
   (**)

PROCEDURE CLEARBUFFER;
VAR
   I : INTEGER;
BEGIN
   FOR I:= 1 TO SIZEOF(G_BUFFER) DO
      G_BUFFER[I]:= 0;
END;

PROCEDURE OUT_DRUCKMASK;
BEGIN
   CLEARFRAME;
   LOWVIDEO;
   GOTOXY(1,4);WRITE('DRUCKFORMAT NR.:');
   NORMVIDEO;
   GOTOXY(1,5);WRITE(CONSTR('*',79));
   LOWVIDEO;
   GOTOXY(1,6);WRITE('ZEITSCHRIFT:');
   GOTOXY(1,10);WRITE('BUCH:');
   GOTOXY(1,14);WRITE('SAMMELBAND:');
   NORMVIDEO;
   GOTOXY(1,18);
   WRITE(CONSTR('*',79));
   LOWVIDEO;
   GOTOXY(1,19);WRITE('SEITENLAENGE:');
   GOTOXY(22,19);WRITE('OBERER RAND:');
   GOTOXY(42,19);WRITE('  ZEILENZAHL:');
   GOTOXY(1,20);WRITE(' LINKER RAND:');
   GOTOXY(22,20);WRITE(' EINRUECKEN:');
   GOTOXY(42,20);WRITE('RECHTER RAND:');
   GOTOXY(1,21);WRITE('   DRUCKSTOP:');
   GOTOXY(22,21);WRITE('   FORMFEED:');
   NORMVIDEO;
END;   (* PROCEDURE OUT_DRUCKMASK *)

PROCEDURE FILL_TXT(I : INTEGER);
VAR
   CNT,A,Y,
   INP       : INTEGER;
   TXTEND    : BOOLEAN;
BEGIN
   INIT_A_PTR;
   CNT:= 1;
   INP:= 1;
   TXTEND:= FALSE;
   INPUT_REC(AKT_AUSG[I][1],238,79,CNT,INP,TXTEND);
   Y:= 6 + (I - 1) * 4;
   A_AKTPTR:= A_SCREENPTR;
   FOR A:= 1 TO 3 DO
   BEGIN
      GOTOXY(1,Y + A);
      IF A_AKTPTR <> NIL THEN
      BEGIN
         WRITE(A_AKTPTR^.LINE);CLREOL;
         GOTOXY(80,Y + A);
         WRITE(A_AKTPTR^.RTN);
         A_AKTPTR:= A_AKTPTR^.NEXT;
      END
      ELSE
         CLREOL;
   END;
   RELEASE(A_HEAPTOP);
END;  (* PROCEDURE FILL_TXT *)

PROCEDURE FILL_DRUCKMASK;
BEGIN
   GOTOXY(18,4);WRITE(AKT_FORMAT_NR);
   FOR I:= 1 TO 3 DO
      FILL_TXT(I);

   GOTOXY(15,19);WRITE(AKT_DRUCK_PARM.SEITENLAENGE);
   GOTOXY(35,19);WRITE(AKT_DRUCK_PARM.OBERER_RAND);
   GOTOXY(56,19);WRITE(AKT_DRUCK_PARM.ZEILENZAHL);
   GOTOXY(15,20);WRITE(AKT_DRUCK_PARM.LINKER_RAND);
   GOTOXY(35,20);WRITE(AKT_DRUCK_PARM.EINRUECK);
   GOTOXY(56,20);WRITE(AKT_DRUCK_PARM.RECHTER_RAND);
   GOTOXY(15,21);
   IF AKT_DRUCK_PARM.DRUCKSTOP THEN
      WRITE('YES')
   ELSE WRITE('NO ');
   GOTOXY(35,21);
   IF AKT_DRUCK_PARM.FORMFEED THEN
      WRITE('YES')
   ELSE WRITE('NO ');
END;    (* PROCEDURE FILL_DRUCKMASK *)

PROCEDURE CLEAR_DRUCKMASK;
VAR
   Y: INTEGER;
BEGIN
   GOTOXY(17,4);CLREOL;
   FOR I:= 1 TO 3 DO
   BEGIN
      Y:= 7 + (I - 1) * 4;
      GOTOXY(1,Y);CLREOL;
      GOTOXY(1,Y + 1);CLREOL;
      GOTOXY(1,Y + 2);CLREOL;
   END;
   FOR I:= 1 TO 3 DO
   BEGIN
      GOTOXY(15,18 + I);WRITE('   ');
      GOTOXY(35,18 + I);WRITE('   ');
      GOTOXY(56,18 + I);WRITE('   ');
   END;
END;  (* PROCEDURE CLEAR_DRUCKMASK *)

PROCEDURE SET_DRUCK_PARM;
VAR
   I : INTEGER;
BEGIN
   IF AKT_FORMAT_NR <= 2 THEN
   BEGIN
      IF AKT_FORMAT_NR = 1 THEN
         SET_DRUCK_PARM_1
      ELSE
         SET_DRUCK_PARM_2;
   END
   ELSE
   BEGIN
      FOR I:= 1 TO 3 DO
      BEGIN
         GETREC(PARMF,(AKT_FORMAT_NR - 3) * 3 + I + 1,G_BUFFER);
         MOVE(G_BUFFER,AKT_AUSG[I][1],238);
         AKT_AUSG[I][0]:= CHR(238);
         AKT_AUSG[I][0]:= CHR(POS(#$1A,AKT_AUSG[I]) - 1);
      END;
      AKT_DRUCK_PARM:= FORM_BUF.P;
   END;
   CLEAR_DRUCKMASK;
   FILL_DRUCKMASK;
END;  (* PROCEDURE SET_DRUCK_PARM *)

PROCEDURE MELDRELEASE;
VAR
   CH : CHAR;
BEGIN
   GOTOXY(1,23);WRITE('SPEICHER ZU KLEIN !');CLREOL;
   READ(KBD,CH);
   RELEASE(HEAPTOP);
   HEAP_RELEASED:= TRUE;
END;     (* END MELDRELEASE *)

OVERLAY PROCEDURE MAKE_LIST;
(**************************)

PROCEDURE GET_LIST(    RECNR    : INTEGER; (* EINLESEN DER VERWEISE IN EINE *)
                   VAR FIRSTTIT : TITPTR); (* LISTE, DER BEGINN DER LISTE *)
VAR                                        (* WIRD IN FIRSTTIT UEBERGEBEN *)
   I       : INTEGER;
   NEWTIT,
   LASTTIT : TITPTR;
BEGIN
   FIRSTTIT:= NIL;
   REPEAT
      GETREC(VERFILE,RECNR,VERWEIS);
      I:= 1;
      WHILE (I <= ANZVER) AND (VERWEIS.DVR[I] <> 0) DO
      BEGIN
         IF VERWEIS.DVR[I] <> -1 THEN (* NUR WENN NICHT GELOESCHT *)
         BEGIN
           IF ABS(MEMAVAIL) < 5 THEN   (* FREIEN PLATZ AUF HEAP PRUEFEN *)
              BEGIN
              MELDRELEASE; EXIT;
              END;
           NEW(NEWTIT);
           NEWTIT^.NR:=VERWEIS.DVR[I];
           NEWTIT^.DISK:=VERWEIS.DVD[I];
           IF FIRSTTIT = NIL THEN
           BEGIN
              FIRSTTIT:=NEWTIT;
              LASTTIT:=NEWTIT;
              FIRSTTIT^.NEXT:= NIL;
           END
           ELSE
             BEGIN
             LASTTIT^.NEXT:= NEWTIT;
             LASTTIT:= NEWTIT;
             LASTTIT^.NEXT:= NIL;
           END;
         END;
         I:= I + 1;
      END;              (* END WHILE *)
      RECNR:= VERWEIS.FOLGE;
   UNTIL RECNR = 0;      (* VERWEISE ZUM STICHWORT SIND JETZT *)
                         (* EINGELESEN, BEGINN DER LISTE IN FIRSTTIT *)
                         (* UEBERGEBEN *)
END; (* PROCEDURE GET_LIST *)

BEGIN (* PROCEDURE MAKE_LIST *)

   (***************************************************************)
   (*      EINLESEN DER VERWEISE AUF DIE TITEL-DATEI              *)
   (***************************************************************)

   GET_LIST(RECNR1,FIRSTTIT); (* VERWEISE ZUM ERSTEN STICHWORT EINLESEN *)
                              (* BEGINN DER 1. LISTE STEHT IN FIRSTTIT  *)
   IF HEAP_RELEASED THEN EXIT;

   IF (RECNR2 <> 0) THEN
   BEGIN
      GET_LIST(RECNR2,FIRSTTIT2); (* VERWEISE ZUM ZWEITEN STICHWORT EINLESEN *)
                                  (* BEGINN DER 2. LISTE STEHT IN FIRSTTIT2  *)
      IF HEAP_RELEASED THEN EXIT;
      (****************************************************************)
      (*       VERKNUEPFUNG DER VERWEISE                              *)
      (****************************************************************)
         NEW(LASTTIT);
         NEWTIT:=FIRSTTIT;
         LASTTIT^.NEXT:=FIRSTTIT;
         NEW(PRETIT2);
                                 (* INITIALISIERUNG BEENDET *)
         WHILE NEWTIT <> NIL DO
            BEGIN
            NEWTIT2:=FIRSTTIT2;
            LASTTIT2:=PRETIT2;
            LASTTIT2^.NEXT:=FIRSTTIT2;
            MATCH:=FALSE;
            REPEAT
              IF NEWTIT^.NR = NEWTIT2^.NR THEN
              BEGIN         (* ^.NR GLEICH *)
                MATCH:= TRUE;
                IF NEWTIT2 = FIRSTTIT2 THEN
                BEGIN  (* VERWEISE AUF GLEICHEN TITEL U. ERSTER VERWEIS IN 2 *)
                  FIRSTTIT2:= NEWTIT2^.NEXT; (* ERSTEN VERWEIS LOESCHEN *)
                  NEWTIT2:= NEWTIT2^.NEXT;
                END
                ELSE
                BEGIN  (* VERWEISE AUF GLEICHEN TITEL UND NICHT ERSTER *)
                       (* VERWEIS IN 2 *)
                LASTTIT2^.NEXT:=NEWTIT2^.NEXT; (* AKTUELLEN VERWEIS IN 2 *)
                                               (* LOESCHEN *)
                NEWTIT2:= NEWTIT2^.NEXT;
                END;     (* END NICHT ERSTER VERWEIS IN 2 *)
              END        (* END ^.NR GLEICH *)
              ELSE       (* ^.NR NICHT GLEICH *)
              BEGIN
                LASTTIT2:= NEWTIT2;
                NEWTIT2:= NEWTIT2^.NEXT;
              END;       (* END ^.NR NICHT GLEICH *)
            UNTIL (NEWTIT2 = NIL) OR (MATCH = TRUE); (* END REPEAT *)
         (*********************************************************)
         (* JETZT BEARBEITUNG IN ERSTER VERWEIS DATEI             *)
         (* WENN KEINE UEBEREINSTIMMUNG MIT EINEM ELEMENT DER     *)
         (* ZWEITEN VERWEIS-LISTE GEFUNDEN WURDE, WIRD DAS BE-    *)
         (* TREFFENDE ELEMENT IN DER 1. LISTE GELOESCHT           *)
         (*********************************************************)
            IF MATCH THEN
            BEGIN         (* UEBEREINSTIMMUNG GEFUNDEN *)
              LASTTIT:= NEWTIT;
              NEWTIT:= NEWTIT^.NEXT;
            END           (* END UEBEREINSTIMMUNG GEFUNDEN *)
            ELSE
            BEGIN         (* KEINE UEBEREINSTIMMUNG GEFUNDEN *)
               IF OPERATOR = '*' THEN
               BEGIN
                 IF NEWTIT = FIRSTTIT THEN
                 BEGIN
                   FIRSTTIT:= NEWTIT^.NEXT;
                   NEWTIT:= FIRSTTIT;
                 END         (* END KEIN MATCH U. 1. ELEMENT IN 1 *)
                 ELSE
                 BEGIN       (* KEIN MATCH U. NICHT 1. ELEMENT IN 1 *)
                   LASTTIT^.NEXT:= NEWTIT^.NEXT;
                   NEWTIT:= NEWTIT^.NEXT;
                 END;    (* END KEIN MATCH U. NICHT 1. ELEMENT IN 1 *)
               END
               ELSE
               BEGIN
                  LASTTIT:=NEWTIT;
                  NEWTIT:= LASTTIT^.NEXT;
               END;
            END;      (* END KEINE UEBEREINSTIMMUNG GEFUNDEN *)
         END;         (* END WHILE *)
   IF OPERATOR = '+' THEN
   LASTTIT^.NEXT:= FIRSTTIT2;
   END;         (* ENDE IF RECNR2 <> 0 *)
END; (* PROCEDURE MAKE_LIST *)

OVERLAY PROCEDURE MODIFIZIEREN;
(*****************************)
VAR
   B_L      : INTEGER;
   TEMP_STR : STR5;
   CH       : CHAR;
   PARMS_OK : BOOLEAN;

   PROCEDURE MODY_TXT(SWITCH : INTEGER);
   VAR
      CNT,
      INP      : INTEGER;
      TXTEND   : BOOLEAN;
   BEGIN
      INIT_A_PTR;
      CNT:= 1;
      INP:= 1;
      TXTEND:= FALSE;
      INPUT_REC(AKT_AUSG[SWITCH][1],238,79,CNT,INP,TXTEND);
      READ_OK:= TRUE;
      INITSCREEN(7 + (SWITCH - 1) * 4,9 + (SWITCH - 1) * 4,0,79,'.');
      ACT_SCREEN(7 + (SWITCH - 1) * 4,9 + (SWITCH - 1) * 4,0,79,'.');
      INPUTSCREEN(7 + (SWITCH - 1) * 4,9 + (SWITCH - 1) * 4,0,79,6,'.',
                  A_TC,TRUE);
      A_AKTPTR:= A_SCREENPTR;
      AKT_AUSG[SWITCH]:= A_AKTPTR^.LINE;
      FOR I:= 1 TO 2 DO
      IF A_AKTPTR^.NEXT <> NIL THEN
      BEGIN
         A_AKTPTR:= A_AKTPTR^.NEXT;
         AKT_AUSG[SWITCH]:= AKT_AUSG[SWITCH] + A_AKTPTR^.LINE;
      END;
      AKT_AUSG[SWITCH]:= AKT_AUSG[SWITCH] + #$1A;
      AKT_AUSG[SWITCH][0]:= PRED(AKT_AUSG[SWITCH][0]);
      RELEASE(A_HEAPTOP);

      FOR I:= 1 TO 3 DO
         FILL_TXT(I);

  END;    (* PROCEDURE MODY_TXT *)

   PROCEDURE MODY_ZAHL(VAR ZAHL          : INTEGER;
                          C_X,C_Y        : INTEGER);
   VAR
      B_OK     : BOOLEAN;
   BEGIN   (* PROCEDURE MODY_ZAHL *)
      REPEAT
        STR(ZAHL,TEMP_STR);
        IF LENGTH(TEMP_STR) > 3 THEN
           TEMP_STR[0]:= CHR(3);
        P:= 0;
        INPUTSTR(TEMP_STR,3,C_X,C_Y,[#$0D,#$05,#$18,#$02],A_TC,2,P,'.',US_T);
        IF (TEMP_STR = '0') OR (TEMP_STR = '  0') OR (TEMP_STR = '000') THEN
        BEGIN
           ZAHL:= 0; B_OK:= TRUE;
        END
        ELSE
           STR5_TO_INTEGER(TEMP_STR,ZAHL,B_OK,TEMP_STR,RZAHL);
        GOTOXY(C_X + 1,C_Y + 1);WRITE('   ');
        GOTOXY(C_X + 1,C_Y + 1);WRITE(ZAHL);
      UNTIL B_OK;
   END; (* PROCEDURE MODY_ZAHL *)

BEGIN     (* PROCEDURE MODIFIZIEREN *)
   IF (AKT_FORMAT_NR = 1) OR (AKT_FORMAT_NR = 2) THEN
   BEGIN
      GOTOXY(1,23);
      WRITE('DIE DRUCKFORMATE NR. 1 UND NR 2 KOENNEN NICHT ');
      WRITE('VERAENDERT WERDEN !');
      CLREOL;
      READ(KBD,CH);
      EXIT;
   END;
   B_L:= 1;
   PROMPT:= 'MODIFIZIEREN: <CR>, '+ #$18 + ', '+ #$19 + ', ' + #$1B +
            ', ' + #$1A + ', CTRL '+ #$1B;
   PROMPT:= PROMPT + ', CTRL ' + #$1A + ', CTRL Y, <END>';
   REPEAT         (* UNTIL PARMS_OK *)
      GOTOXY(1,23);
      WRITE(PROMPT);CLREOL;
      REPEAT
         CASE B_L OF
       1 : MODY_TXT(1);
       2 : MODY_TXT(2);
       3 : MODY_TXT(3);
       4 : MODY_ZAHL(AKT_DRUCK_PARM.SEITENLAENGE,14,18);
       5 : MODY_ZAHL(AKT_DRUCK_PARM.OBERER_RAND,34,18);
       6 : MODY_ZAHL(AKT_DRUCK_PARM.ZEILENZAHL,55,18);
       7 : MODY_ZAHL(AKT_DRUCK_PARM.LINKER_RAND,14,19);
       8 : MODY_ZAHL(AKT_DRUCK_PARM.EINRUECK,34,19);
       9 : MODY_ZAHL(AKT_DRUCK_PARM.RECHTER_RAND,55,19);
      10 : BEGIN
              P:= 0;
              IF AKT_DRUCK_PARM.DRUCKSTOP THEN
                 TEMP_STR:= 'YES'
              ELSE
                 TEMP_STR:= 'NO ';
              INPUTSTR(TEMP_STR,3,14,20,[#$0D,#$05,#$18,#$02],
                       A_TC,3,P,'.',US_T);
              IF TEMP_STR = 'YES' THEN
                 AKT_DRUCK_PARM.DRUCKSTOP:= TRUE
              ELSE
              BEGIN
                 AKT_DRUCK_PARM.DRUCKSTOP:= FALSE;
                 GOTOXY(15,21);WRITE('NO ');
              END;
           END;
      11 : BEGIN
              P:= 0;
              IF AKT_DRUCK_PARM.FORMFEED THEN
                 TEMP_STR:= 'YES'
              ELSE
                 TEMP_STR:= 'NO ';
              INPUTSTR(TEMP_STR,3,34,20,[#$0D,#$05,#$18,#$02],
                       A_TC,3,P,'.',US_T);
              IF TEMP_STR = 'YES' THEN
                 AKT_DRUCK_PARM.FORMFEED:= TRUE
              ELSE
              BEGIN
                 AKT_DRUCK_PARM.FORMFEED:= FALSE;
                 GOTOXY(35,21);WRITE('NO ');
              END;
           END;
         END;  (* CASE *)
         IF A_TC = #$05 THEN    (* LINE UP *)
         BEGIN
            IF B_L = 1 THEN
               B_L:= 11
            ELSE
               B_L:= B_L - 1;
         END;
         IF (A_TC = #$0D) OR (A_TC = #$18) THEN   (* CR ODER LINE DOWN *)
         BEGIN
            IF B_L = 11 THEN
               B_L:= 1
            ELSE
               B_L:= B_L + 1;
         END;
      UNTIL A_TC = #$02;
      PARMS_OK:= TRUE;
      WITH AKT_DRUCK_PARM DO
      BEGIN
         IF (SEITENLAENGE < 2) OR (SEITENLAENGE > 255) THEN
            PARMS_OK:= FALSE;
         IF (OBERER_RAND < 0) OR (OBERER_RAND > 253) THEN
            PARMS_OK:= FALSE;
         IF (ZEILENZAHL < 2) OR (ZEILENZAHL > 255) THEN
            PARMS_OK:= FALSE;
         IF ZEILENZAHL + OBERER_RAND > SEITENLAENGE THEN
            PARMS_OK:= FALSE;
         IF (LINKER_RAND < 1) OR (LINKER_RAND > 100) THEN
            PARMS_OK:= FALSE;
         IF (RECHTER_RAND < LINKER_RAND + 10) OR (RECHTER_RAND > 150) THEN
            PARMS_OK:= FALSE;
         IF (LINKER_RAND + EINRUECK > RECHTER_RAND - 10) OR  (RECHTER_RAND -
             LINKER_RAND > 120) THEN
            PARMS_OK:= FALSE;
      END; (* WITH *)
      IF NOT PARMS_OK THEN
      BEGIN
         GOTOXY(1,23);
         WRITE('UNSINNIGE DRUCKPARAMETER !'); CLREOL;
         BEEP;
         READ(KBD,CH);
      END;
   UNTIL PARMS_OK;
END;   (* PROCEDURE MODIFIZIEREN *)
         (*****************************)


(****************************************)
(*                                      *)
       PROCEDURE DRUCK_FORMAT;
(*                                      *)
(****************************************)
VAR
   CH        : CHAR;


PROCEDURE NEU;
(************)
BEGIN
   AKT_FORMAT_NR:= (PARMF.NUMREC DIV 3) + 3;
   GOTOXY(18,4);WRITE(AKT_FORMAT_NR);CLREOL;
END;
         (*****************************)

PROCEDURE VORHERIGER;
(*******************)
BEGIN
   IF AKT_FORMAT_NR = 1 THEN
      AKT_FORMAT_NR:= PARMF.NUMREC DIV 3 + 2
   ELSE
      AKT_FORMAT_NR:= AKT_FORMAT_NR - 1;
   SET_DRUCK_PARM;
END;
         (*****************************)

PROCEDURE NAECHSTER;
(******************)
BEGIN
   IF AKT_FORMAT_NR >= (PARMF.NUMREC DIV 3 + 2) THEN
      AKT_FORMAT_NR:= 1
   ELSE
      AKT_FORMAT_NR:= AKT_FORMAT_NR + 1;
   SET_DRUCK_PARM;
END;
         (*****************************)

PROCEDURE SPEICHERN;
(******************)
VAR
   I,A : INTEGER;
BEGIN
   IF AKT_FORMAT_NR <= 2 THEN
      EXIT;
   FOR I:= 1 TO 3 DO
   BEGIN
      CLEARBUFFER;
      MOVE(AKT_AUSG[I][1],G_BUFFER,LENGTH(AKT_AUSG[I]));
      G_BUFFER[LENGTH(AKT_AUSG[I]) + 1]:= $1A;
      FORM_BUF.P:= AKT_DRUCK_PARM;
      A:= ((AKT_FORMAT_NR - 3) * 3 + I + 1);
      IF A >= PARMF.NUMREC THEN
         ADDREC(PARMF,A,G_BUFFER)
      ELSE
         PUTREC(PARMF,A,G_BUFFER);
   END;
END;
         (*****************************)

PROCEDURE AKZEPTIEREN;
(********************)

PROCEDURE SEARCH_ITEMS;   (* ITEMS (Z. B. <NR> ODER <TITEL> IN AUSG SUCHEN *)
VAR
   I,A   : INTEGER;
   START : BYTE;
BEGIN
   FOR I:= 1 TO 3 DO
   BEGIN
      CASE I OF
        1: BEGIN
              ITEM_NR[8]:= '<ZEITSCHR>';
              ITEM_NR[9]:= '<BAND>';
              ITEM_NR[10]:= '';
              ITEM_NR[11]:= '';
           END;
        2: BEGIN
              ITEM_NR[8]:= '<BVERLAG>';
              ITEM_NR[9]:= '<BORT>';
              ITEM_NR[10]:= '';
              ITEM_NR[11]:= '';
           END;
        3: BEGIN
              ITEM_NR[8]:= '<HRSG>';
              ITEM_NR[9]:= '<HRSGTITEL>';
              ITEM_NR[10]:= '<SVERLAG>';
              ITEM_NR[11]:= '<SORT>';
           END;
        END;  (* CASE *)

      FOR A:= 1 TO 11 DO
      BEGIN
         ITEM_TAB[I][A].C.ITEM:= A;
         START:= POS(ITEM_NR[A],AKT_AUSG[I]);
         IF (START = 0) OR (ITEM_NR[A] = '') THEN
            START:= $FF;
         ITEM_TAB[I][A].C.START:= START;
         ITEM_TAB[I][A].C.LAENGE:= LENGTH(ITEM_NR[A]);
         ITEM_TAB[I][A].PRE_STR:= '';
      END; (* FOR A:= 1 TO 11 *)
   END; (* FOR I:= 1 TO 3 *)
END;  (* END PROCEDURE SEARCH_ITEMS *)

PROCEDURE SORT_ITEMS;    (* ITEMS NACH DER FOLGE IM STRING "AUSG" SORTIEREN *)
VAR
   ZWISCHEN : CO;
   I,A      : INTEGER;
   CHANGED  : BOOLEAN;
BEGIN
   FOR I:= 1 TO 3 DO
      REPEAT
         CHANGED:= FALSE;
         FOR A:= 1 TO 10 DO
            IF ITEM_TAB[I][A].C.START > ITEM_TAB[I][A + 1].C.START THEN
            BEGIN
               ZWISCHEN:= ITEM_TAB[I][A].C;
               ITEM_TAB[I][A].C:= ITEM_TAB[I][A + 1].C;
               ITEM_TAB[I][A + 1].C:= ZWISCHEN;
               CHANGED:= TRUE;
            END;
      UNTIL NOT CHANGED;
END; (* END PROCEDURE SORT_ITEMS *)

PROCEDURE GET_PRESTRINGS;   (* TEXT ZWISCHEN DEN ITEMS IN PRE_STR EINTRAGEN *)
VAR
   I,A      : INTEGER;
   LAST_END : INTEGER;
BEGIN
   FOR I:= 1 TO 3 DO
   BEGIN
      A:= 1;
      REPEAT
         IF A = 1 THEN
            LAST_END:= 1
         ELSE
            LAST_END:= ITEM_TAB[I][A-1].C.START + ITEM_TAB[I][A-1].C.LAENGE;
         ITEM_TAB[I][A].PRE_STR:= COPY(AKT_AUSG[I],LAST_END,
                                       ITEM_TAB[I][A].C.START - LAST_END);
         A:= A + 1;
      UNTIL (A = 12) OR (ITEM_TAB[I][A].C.START = $FF);
      LAST_END:= ITEM_TAB[I][A-1].C.START + ITEM_TAB[I][A-1].C.LAENGE;
      POST_STR[I]:= COPY(AKT_AUSG[I],LAST_END,LENGTH(AKT_AUSG[I]) - LAST_END + 1);
   END; (* END FOR I:= 1 TO 3 DO *)
END; (* END PROCEDURE GET_PRESTRINGS *)

BEGIN  (* PROCEDURE AKZEPTIEREN *)
   SEARCH_ITEMS;
   SORT_ITEMS;
   GET_PRESTRINGS;
END; (* PROCEDURE AKZTEPIEREN *)



BEGIN    (* BEGIN DRUCK_FORMAT *)
   CLEARFRAME;
   OUT_DRUCKMASK;
   FILL_DRUCKMASK;
   REPEAT
      PROMPT:= 'DRUCKFORMAT: M)ODIFIZIEREN, N)EU, ' + #$1B +
               ', ' + #$1A + ', S)PEICHERN, A)KZEPTIEREN';
      SELECT(PROMPT,['M','N',#$13,#$04,'S','A',#$0D],CH,'A');
      CASE CH OF
        'M'  : MODIFIZIEREN;
        'N'  : NEU;
        #$13 : VORHERIGER;
        #$04 : NAECHSTER;
        'S'  : SPEICHERN;
        'A'  : AKZEPTIEREN;
      END; (* CASE *)
   UNTIL CH = 'A';
END;    (* PROCEDURE DRUCK_FORMAT *)

PROCEDURE AUSG_GESAMT;
VAR
   SHORT_SCR : SCREENREC ABSOLUTE SCR;
PROCEDURE PREPARE_AUSG;
VAR
   I     : INTEGER;
BEGIN
   AUSG_UNIT_BEREIT:= TRUE;
   ABORTED:= FALSE;
   CONT:= 0;
   EPTR:= 1;
   APTR:= 1;
   FOR I:= 1 TO 100 DO
      DR_BUF[I]:= '';
   DR_CONT:= 0;
   DR_PTR:= 1;
   EINRUECK:= AKT_DRUCK_PARM.EINRUECK;
   LINE_ZEIGER:= 1;
   CASE AUSG_ZIEL OF
      'S': BEGIN
              LINE_LENGTH:= 79;
              MAX_LINE:= 17;
           END;
  'P','D': BEGIN
              LINE_LENGTH:= AKT_DRUCK_PARM.RECHTER_RAND -
                            AKT_DRUCK_PARM.LINKER_RAND + 1;
              MAX_LINE:= AKT_DRUCK_PARM.ZEILENZAHL;
           END;
    END; (* CASE *)
END;
BEGIN   (* PROCEDURE AUSG_GESAMT *)
   PREPARE_AUSG;
   CASE SORTIERT OF
      TRUE  : BEGIN
                 SORT_RESULT:= TURBOSORT(SIZEOF(SCREENREC));
                 IF SORT_RESULT <> 0 THEN
                 BEGIN
                    GOTOXY(1,23);
                    WRITE('SORTIERFEHLER NR: ',SORT_RESULT,' AUFGETRETEN !');
                    CLREOL;
                 END;
                 IF (NOT ABORTED) AND (AUSG_ZIEL = 'S') THEN
                    READ(KBD,ABO_CH);
              END;
      FALSE : BEGIN
                 NEWTIT:= FIRSTTIT;
                 REPEAT
                    IF (CONT < 16) AND (NEWTIT <> NIL) THEN
                    BEGIN
                       IF NEWTIT^.DISK = DISKNR THEN
                       BEGIN
                          GETREC(DATF,NEWTIT^.NR,DATREC);
                          RECONSTRUCT(SCR);
                          SCR.C.REC_NR:= NEWTIT^.NR;
                          SCR.C.ART:= DATREC.ART;
                          SCR_BUF[EPTR]:= SHORT_SCR;
                          CONT:= CONT + 1;
                          IF EPTR = 16 THEN
                             EPTR:= 1
                          ELSE
                             EPTR:= SUCC(EPTR);
                       END;
                       NEWTIT:= NEWTIT^.NEXT;
                    END;
                    AUSGABE(AUSG_UNIT_BEREIT,ABORTED);
                 UNTIL (NEWTIT = NIL) AND (AUSG_UNIT_BEREIT) OR ABORTED;
                 IF (NOT ABORTED) AND (AUSG_ZIEL = 'S') THEN
                    READ(KBD,ABO_CH);
                 IF AUSG_ZIEL = 'D' THEN
                    CLOSE(TEXTF);
              END;   (* FALSE: *)
    END; (* CASE *)
END;  (* PROCEDURE AUSG_GESAMT *)
PROCEDURE GETKW;
CONST
   TERM : CHARSET = [#$05,#$0D];
VAR
   L  : INTEGER;
   TC : CHAR;
BEGIN
   CLEARFRAME;
   GOTOXY(1,5); WRITE('KEY 1 :');
   GOTOXY(36,5); WRITE('+/* :      KEY 2 :');
   KW1:= '';
   KW2:= '';
   OPERATOR:= '';
   L:=1;
   REPEAT
      P:= 0;
      CASE L OF
        1 : INPUTSTR(KW1,URKEYLEN,8,4,TERM,TC,1,P,'.',US_T);
        2 : INPUTSTR(OPERATOR,1,41,4,TERM,TC,0,P,'.',US_T);
        3 : INPUTSTR(KW2,URKEYLEN,54,4,TERM,TC,1,P,'.',US_T);
      END;       (* END CASE *)
      IF TC = #$0D THEN (* CR *)
        IF L = 3 THEN
           L:=1
        ELSE  L:= L + 1
      ELSE
        IF TC = #$05 THEN       (* CTRL-E = LINE UP *)
           IF L = 1 THEN
              L:= 3
        ELSE L:= L -1;
   UNTIL ((TC = #$0D) AND (L = 1))
         AND (OPERATOR[1] IN ['*','+',#$00]);    (* CR, CTRL-Z *)
   END;
   (**)             (* END PROCEDURE GETKW *)






BEGIN               (* BEGIN PROCEDURE SUCHE *)
  DRUCK_FORMAT;
  SELECT('AUSGABE AUF P)RINTER, D)ATEI ODER S)CREEN ?',[#$0D,'P','D','S'],
          CH,'S');
  CASE CH OF
      'S' : AUSG_ZIEL:= 'S';
      'P' : AUSG_ZIEL:= 'P';
      'D' : AUSG_ZIEL:= 'D';
    END;  (* CASE *)

  IF AUSG_ZIEL = 'D' THEN
  BEGIN
     SELECT('DATEIAUSGABE: W)ORDSTAR ODER A)SCII DATEI ?',
            ['W','A'],CH,' ');
     CASE CH OF
        'W': WORDSTAR:= TRUE;
        'A': WORDSTAR:= FALSE;
       END; (* CASE *)
     TEXTDATEI:= '';
     REPEAT
        GOTOXY(1,23);
        WRITE('NAME DER AUSGABEDATEI:');
        CLREOL;
        P:= 0;
        INPUTSTR(TEXTDATEI,30,23,22,[#$0D],CH,7,P,'.',US_T);
        ASSIGN(TEXTF,TEXTDATEI);
        (*$I-*)
        RESET(TEXTF);
        (*$I+*)
        IF (IORESULT = 0) THEN
           SELECT('DATEI GLEICHEN NAMENS EXISTIERT. UEBERSCHREIBEN (Y/N) ?',
                  ['Y','N'],CH,' ')
        ELSE
           CH:= 'Y';
     UNTIL CH = 'Y';
     REWRITE(TEXTF);
     IF WORDSTAR THEN
     BEGIN
        WRITE(TEXTF,'..Uebernehmen der Kopfzeile mit Ctrl-O-F',#$0D);
        FOR I:= 2 TO AKT_DRUCK_PARM.LINKER_RAND DO
           WRITE(TEXTF,#$20);
        FOR I:= 1 TO AKT_DRUCK_PARM.EINRUECK DO
           WRITE(TEXTF,#$2D);
        WRITE(TEXTF,#$21);
        I:= AKT_DRUCK_PARM.LINKER_RAND + AKT_DRUCK_PARM.EINRUECK + 1;
        WHILE I <= AKT_DRUCK_PARM.RECHTER_RAND DO
        BEGIN
           IF (I MOD 10) = 0 THEN
              WRITE(TEXTF,#$21)
           ELSE
              WRITE(TEXTF,#$2D);
           I:= SUCC(I);
        END;
        WRITE(TEXTF,#$0D#$0A);
        STR(AKT_DRUCK_PARM.SEITENLAENGE,DR_STR);   (* SEITENLAENGE *)
        WRITE(TEXTF,'.PL',DR_STR,#$0D,#$0A);
        STR(AKT_DRUCK_PARM.OBERER_RAND,DR_STR);    (* OBERER_RAND *)
        WRITE(TEXTF,'.MT',DR_STR,#$0D,#$0A);
        UNTERER_RAND:= AKT_DRUCK_PARM.SEITENLAENGE -
            (AKT_DRUCK_PARM.OBERER_RAND + AKT_DRUCK_PARM.ZEILENZAHL);
        STR(UNTERER_RAND,DR_STR);
        WRITE(TEXTF,'.MB',DR_STR,#$0D,#$0A);        (* UNTERER RAND *)
        WRITE(TEXTF,'..');
        FOR I:= 1 TO 60 DO
           WRITE(TEXTF,'*');
        WRITE(TEXTF,#$0D#$0A);
     END;
  END; (* IF AUSG_ZIEL = 'D' *)
  SELECT('AUSGABE: S)ORTIERT ODER U)NSORTIERT ?',[#$0D,'S','U'],CH,'U');
  CASE CH OF
      'S': SORTIERT:= TRUE;
      'U': SORTIERT:= FALSE;
    END;  (* CASE *)
  IF SORTIERT THEN
  BEGIN
     SELECT('SORTIERT NACH: N)UMMER, A)UTOREN, T)ITEL, J)AHR, S)ORT-FELD ?',
            [#$0D,'N','A','T','J','S'],CH,'A');
     CASE CH OF
        'N' : SORT_ITEM:= 1;
        'A' : SORT_ITEM:= 3;
        'T' : SORT_ITEM:= 2;
        'J' : SORT_ITEM:= 4;
        'S' : SORT_ITEM:= 5;
      END; (* CASE *)
  END;
  REPEAT
    GETKW;
    IF KW1 <> '' THEN
    BEGIN
       PACK(KW1,KWINT);
       FINDKEY(NAMEINDEXFILE,DATAF,KWINT);
       IF OK THEN        (* STICHWORT 1 GEFUNDEN *)
          RECNR1:= DATAF
       ELSE RECNR1:=0;   (* STICHWORT 1 NICHT GEFUNDEN *)
    END
    ELSE RECNR1:= 0;
    IF KW2 <> '' THEN
    BEGIN
       PACK(KW2,KWINT);
       FINDKEY(NAMEINDEXFILE,DATAF,KWINT);
       IF OK THEN        (* STICHWORT 2 GEFUNDEN *)
          RECNR2:= DATAF
       ELSE RECNR2:= 0;
    END
    ELSE RECNR2:= 0;
    (**)
    (* IN RECNR1 UND RECNR2 STEHT JETZT NULL WENN EIN LEERES *)
    (* STICHWORT EINGEGEBEN WURDE, ODER WENN DAS STICHWORT   *)
    (* NICHT GEFUNDEN WURDE                                  *)
    IF RECNR1 = 0 THEN
    BEGIN
       GOTOXY(1,15);WRITE('STICHWORT 1 NICHT GEFUNDEN !'); CLREOL;
       IF RECNR2 = 0 THEN
       BEGIN
         IF KW2 <> '' THEN
         BEGIN
           GOTOXY(1,16); WRITE ('STICHWORT 2 NICHT GEFUNDEN !'); CLREOL;
         END;
       END;
       READ(KBD,TC);
       TC:=#$00;
    END          (* END RECNR1 = 0 *)
    ELSE         (* RECNR1 <> 0 *)
    BEGIN
       IF RECNR2 = 0 THEN
       BEGIN
          IF KW2 <> '' THEN
          BEGIN
             GOTOXY(1,15); WRITE('STICHWORT 2 NICHT GEFUNDEN !');
             CLREOL;
             REPEAT UNTIL KEYPRESSED;
             READ(KBD,TC);
             TC:=#$00;
          END;
       END;     (* END RECNR1 <> 0, RECNR2 = 0 *)
    END;
    UNTIL (RECNR1 <> 0) AND ((RECNR2 <> 0) OR (KW2 = ''));

IF (AUSG_ZIEL = 'P') THEN        (* INITIALISIERUNGSSTRING AN PRINTER *)
   FOR I:= 1 TO LENGTH(PR.DRUCKINITSTR) DO
      CHAROUT(PR.DRUCKINITSTR[I]);

MARK(HEAPTOP);
HEAP_RELEASED:= FALSE;

MAKE_LIST;      (* FALLS BEI BEARBEITUNG VON MAKE_LIST NICHT GENUEGEND *)
                (* PLATZ AUF DEM HEAP WIRD DER HEAP FREIGEGEBEN UND *)
                (* HEAP_RELEASED AUF TRUE GESETZT *)

IF HEAP_RELEASED THEN EXIT;

(***************************************************************)
(*   JETZT STEHEN ALLE VERKNUEPFTEN ELEMENTE IN LISTE 1        *)
(***************************************************************)
(*     EINLESEN DER VORKOMMENDEN DISKETTEN IN LISTE            *)
(***************************************************************)
NEW(LASTDISK);
NEUEDISK:= TRUE;
REPEAT         (* UNTIL NEUEDISK = FALSE *)
NEWTIT:= FIRSTTIT;
FIRSTDISK:= NIL;
DATLIST:=[DISKNR];
WHILE NEWTIT <> NIL DO
BEGIN
   IF (NOT (NEWTIT^.DISK IN DATLIST)) AND (NEWTIT <> NIL) THEN
   BEGIN
      IF ABS(MEMAVAIL) < 5 THEN
         BEGIN
         MELDRELEASE; EXIT;
         END;
      NEW(NEWDISK);
      STR(NEWTIT^.DISK :2,NEWDISK^.NR);
      IF FIRSTDISK = NIL THEN
      BEGIN
        FIRSTDISK:= NEWDISK;LASTDISK:= FIRSTDISK;
        LASTDISK^.NEXT:= NIL;
      END
      ELSE
      BEGIN
        LASTDISK^.NEXT:=NEWDISK;
        LASTDISK:=NEWDISK;
        LASTDISK^.NEXT:=NIL;
      END;
      DATLIST:=DATLIST + [NEWTIT^.DISK];
   END;
   NEWTIT:=NEWTIT^.NEXT;
END;
 (**********************************************)
NEWTIT:=FIRSTTIT;
CLEARFRAME;
AUSG_GESAMT;

(**)
CLEARFRAME;
NEWDISK:= FIRSTDISK;
IF FIRSTDISK <> NIL THEN
BEGIN
   GOTOXY(1,4);
   WRITE('WEITERE TITEL BEFINDEN SICH AUF DATEI(EN):');
   Y:= 5;
   C:= 0;
   REPEAT
      REPEAT
         GOTOXY(C * 5 + 1,Y);WRITE(NEWDISK^.NR,'   ');
         C:= C + 1;
         NEWDISK:= NEWDISK^.NEXT;
      UNTIL (C = 16) OR (NEWDISK = NIL);
      Y:= Y + 1;
   UNTIL NEWDISK = NIL;
   SELECT('NEUE DATEI ODER DISKETTE (Y/N)?',['Y','N',#$0D],CH,'N');
   IF CH = 'Y' THEN
   BEGIN
      REPEAT
        (*$I-*)RESET(DATF.F);(*$I+*)
        OK:=(IORESULT = 0);
        IF NOT OK THEN
        BEGIN
          GOTOXY(1,23);
          WRITE('DISKETTE GEWECHSELT ! ALTE DISKETTE EINLEGEN !');
          CLREOL;
          READ(KBD,TC);
        END;
        UNTIL OK;
      TARECBUF_2[5]:= TITEL_ANZ;
      CLOSEFILE(DATF);
      DAT_OPEN[1]:= FALSE;
      IF ABS_OPEN THEN
      BEGIN
         CLOSEFILE(ABSF);
         ABS_OPEN:= FALSE;
      END;
      GOTOXY(1,23);
      WRITE('NEUE DISKETTE IN LAUFWERK ',TITELDRV,': EINLEGEN !');
      CLREOL;
      CLEARFRAME;
      READ(KBD,CH);

      REPEAT
        REPEAT
           SELECT('INHALTSVERZEICHNIS ZEIGEN (Y/N)?',['Y','N',#$0D],CH,'N');
           IF CH = 'Y' THEN
              DIRECTORY('????????.???',16); (* STANDARDDATEIEN UND *)
        UNTIL CH = 'N';                     (* SUBDIRECTORIES *)
        REPEAT
           OUTCASE;
           ALTIDXPFAD:= PR.IDXPFAD;
           GET_DATNAMES(2);
           PR.IDXPFAD:= ALTIDXPFAD;
           IF (PR.TITPFAD[LENGTH(PR.TITPFAD)] IN ['0'..'9']) AND
              (PR.TITPFAD[LENGTH(PR.TITPFAD) - 1] IN ['0'..'9']) THEN
              OK:= TRUE
           ELSE OK:= FALSE;
        UNTIL OK;
        IF PR.TITPFAD[2] <> ':' THEN
        BEGIN
           TITELDRVNR:= LOGDRVNR;
           TITELDRV:= LOGDRV;
        END
        ELSE
        BEGIN
           TITELDRV:= PR.TITPFAD[1];
           TITELDRVNR:= ORD(TITELDRV) - 65;
        END;
        DISKNUMMER:= COPY(PR.TITPFAD,LENGTH(PR.TITPFAD) - 1,2);
        VAL(DISKNUMMER,I,RESULT);
        DISKNR:= I;

       (* HIER MUSS NOCH SICHERHEIT REIN *)

        GOTOXY(1,23);
        WRITE('TITEL-DATEI IN DRIVE ',TITELDRV,': ERWARTET.');CLREOL;

        CLEARFRAME;
        PFAD:= PR.TITPFAD + DATEXT[1];
        OPENFILE(DATF,PFAD,LITRECSIZE);
        IF NOT OK THEN
        BEGIN
           GOTOXY(1,23);
           WRITE('DATEI "',PR.TITPFAD,'.DTA" NICHT GEFUNDEN.');CLREOL;
           READ(KBD,CH);
        END
        ELSE
        BEGIN
           TITEL_ANZ:= TARECBUF_2[5];
           DAT_OPEN[1]:= TRUE;
        END;
      UNTIL OK;
      GOTOXY(39,2);WRITE(DISKNUMMER);
      GOTOXY(50,2);WRITE(INT_TO_STR5(TITEL_ANZ));
      GETFREEGROUPS;
   END
   ELSE NEUEDISK:= FALSE;
END
ELSE NEUEDISK:= FALSE;
UNTIL (NEUEDISK = FALSE);

RELEASE(HEAPTOP);
END;       (* END PROCEDURE SUCHE *)
