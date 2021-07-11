OVERLAY PROCEDURE  AUSGABE(VAR AUSG_UNIT_BEREIT : BOOLEAN;
(*****************)VAR ABORTED          : BOOLEAN);
CONST
   LEER_STR : STR1 = '';
   END_STR  : STR1 = #$1A;
   FF_STR   : STR4 = '<FF>';
   CR_STR   : STR4 = '<CR>';
VAR
   WORKSTR          : ANYSTR;
   I,A,B,STRICH_POS : INTEGER;

FUNCTION INC_DR_PTR_OK : BOOLEAN;
(********************)
VAR
  I : INTEGER;
BEGIN
   IF DR_PTR < 100 THEN
   BEGIN
      DR_PTR:= SUCC(DR_PTR);
      FOR I:= 1 TO LENGTH(DR_BUF[DR_PTR]) DO
         DR_BUF[DR_PTR][I]:= #$FF;
      DR_BUF[DR_PTR]:= '';
      DR_CONT:= SUCC(DR_CONT);
      INC_DR_PTR_OK:= TRUE;
   END
   ELSE
      INC_DR_PTR_OK:= FALSE;
END;

PROCEDURE PRINTOUT_BUFFER(VAR AUSG_UNIT_BEREIT : BOOLEAN;
(************************)VAR ABORTED          : BOOLEAN);
VAR
   BLK,PRT_CH    : CHAR;
   I             : INTEGER;
(*
   DUMMY         : INTEGER ABSOLUTE $5000:$0002;
   DUMM_CONT     : INTEGER ABSOLUTE $5000:$0000;
*)
BEGIN
(* GOTOXY(1,23);
   WRITE(DR_CONT);CLREOL;
   DELAY(1000);
   DUMM_CONT:= DR_CONT;
   MOVE(DR_BUF,DUMMY,SIZEOF(DR_BUF));
*)
   WHILE (LINE_ZEIGER <= MAX_LINE) AND (DR_CONT <> 0) DO
   BEGIN
      ABO_CH:= #$00;
      CASE AUSG_ZIEL OF
         'S': BEGIN
                 GOTOXY(1,LINE_ZEIGER + 3);
                 IF DR_BUF[DR_PTR][1] = '<' THEN
                 BEGIN
                    IF (DR_BUF[DR_PTR] = '<FF>') OR
                                   (DR_BUF[DR_PTR] = '<STOP>') THEN
                       LINE_ZEIGER:= MAX_LINE
                    ELSE
                       IF DR_BUF[DR_PTR] <> '<CR>' THEN
                          WRITE(DR_BUF[DR_PTR]);
                 END
                 ELSE
                    WRITE(DR_BUF[DR_PTR]);
              END;
         'P': BEGIN
                 IF LINE_ZEIGER = 1 THEN
                    IF (DR_BUF[DR_PTR] <> '<STOP>') AND
                       (DR_BUF[DR_PTR] <> '') AND
                       (DR_BUF[DR_PTR] <> '<FF>') THEN
                       FOR I:= 1 TO AKT_DRUCK_PARM.OBERER_RAND DO
                          WRITE(LST,#$0D#$0A);
                 IF DR_BUF[DR_PTR][1] = '<' THEN
                 BEGIN
                    IF DR_BUF[DR_PTR] = '<CR>' THEN
                       WRITE(LST,#$0D#$0A)
                    ELSE
                       IF DR_BUF[DR_PTR] = '<FF>' THEN
                          IF LINE_ZEIGER > 1 THEN
                          BEGIN
                             IF AKT_DRUCK_PARM.FORMFEED THEN
                                LINE_ZEIGER:= MAX_LINE
                             ELSE
                             BEGIN
                                FOR I:= LINE_ZEIGER TO MAX_LINE DO
                                   WRITE(LST,#$0D#$0A);
                                   LINE_ZEIGER:= MAX_LINE;
                             END;
                          END
                          ELSE
                             LINE_ZEIGER:= PRED(LINE_ZEIGER)
                       ELSE
                          IF DR_BUF[DR_PTR] = '<STOP>' THEN
                          BEGIN
                             READ(KBD,ABO_CH);
                             IF ABO_CH = #$1B THEN
                                LINE_ZEIGER:= MAX_LINE
                             ELSE
                                LINE_ZEIGER:= LINE_ZEIGER - 1;
                          END
                          ELSE
                          BEGIN
                             IF (AKT_DRUCK_PARM.LINKER_RAND > 1) AND
                                      (DR_BUF[DR_PTR] <> '') THEN
                                FOR I:= 2 TO AKT_DRUCK_PARM.LINKER_RAND DO
                                   WRITE(LST,#$20);
                             WRITE(LST,DR_BUF[DR_PTR]);
                          END;
                 END
                 ELSE  (* NOT '<' *)
                 BEGIN
                    IF (AKT_DRUCK_PARM.LINKER_RAND > 1) AND
                       (DR_BUF[DR_PTR] <> '') THEN
                       FOR I:= 2 TO AKT_DRUCK_PARM.LINKER_RAND DO
                          WRITE(LST,#$20);
                    WRITE(LST,DR_BUF[DR_PTR]);
                 END;
              END;  (* END 'P' *)
         'D': BEGIN
                 IF WORDSTAR THEN
                    BLK:= #$A0
                 ELSE
                    BLK:= #$20;
                 IF AKT_DRUCK_PARM.LINKER_RAND > 1 THEN
                    IF DR_BUF[DR_PTR] <> '' THEN
                    FOR I:= 2 TO AKT_DRUCK_PARM.LINKER_RAND DO
                       WRITE(TEXTF,BLK);
                 IF DR_BUF[DR_PTR][1] = '<' THEN
                 BEGIN
                    IF DR_BUF[DR_PTR] = '<CR>' THEN
                       WRITE(TEXTF,#$0D#$0A)
                    ELSE
                       IF DR_BUF[DR_PTR] = '<FF>' THEN
                          IF WORDSTAR THEN
                          BEGIN
                             WRITE(TEXTF,'.PA',#$0D,#$0A);
                             LINE_ZEIGER:= MAX_LINE;
                          END
                          ELSE
                          BEGIN
                             FOR I:= LINE_ZEIGER TO MAX_LINE DO
                             BEGIN
                                WRITE(TEXTF,#$0D#$0A);
                                LINE_ZEIGER:= SUCC(LINE_ZEIGER);
                             END;
                             LINE_ZEIGER:= PRED(LINE_ZEIGER);
                          END;
                 END
                 ELSE
                    FOR I:= 1 TO LENGTH(DR_BUF[DR_PTR]) DO
                    BEGIN
                       PRT_CH:= DR_BUF[DR_PTR][I];
                       IF PRT_CH < #$80 THEN
                          WRITE(TEXTF,PRT_CH)
                       ELSE
                          IF (PRT_CH = #$A0) OR (PRT_CH = #$8A) THEN
                             WRITE(TEXTF,PRT_CH)
                          ELSE
                             IF NOT WORDSTAR THEN
                                WRITE(TEXTF,PRT_CH)
                             ELSE
                                IF PRT_CH IN [#$8E,#$84,#$99,#$94,#$9A,#$81,
                                              #$E1] THEN
                                   WRITE(TEXTF,#$1B,PRT_CH,#$1C);
                    END; (* FOR I:= 1 TO *)
              END;
         END;  (* CASE *)
      IF DR_BUF[DR_PTR] <> '' THEN
         LINE_ZEIGER:= SUCC(LINE_ZEIGER);
      DR_CONT:= PRED(DR_CONT);
      DR_PTR:= SUCC(DR_PTR);
      ADVANCED:= FALSE;
   END; (* WHILE *)

   IF (LINE_ZEIGER > MAX_LINE) THEN
   BEGIN
      IF KEYPRESSED OR (ABO_CH = #$1B) THEN
      BEGIN
         IF KEYPRESSED THEN
            READ(KBD,ABO_CH);
         IF ABO_CH = #$1B THEN       (* ABBRECHEN MIT ESC *)
            ABORTED:= TRUE
         ELSE
            ABORTED:= FALSE;
         AUSG_UNIT_BEREIT:= FALSE;
         LINE_ZEIGER:= 1;
         CASE AUSG_ZIEL OF
            'S': BEGIN
                    CLEARFRAME;
                    GOTOXY(1,4);
                 END;
            'P': BEGIN
                 END;
            'D': BEGIN
                    IF ABO_CH = #$1B THEN
                       WRITE(TEXTF,#$1A);
                 END;
         END; (* CASE *)
      END   (* END IF KEYPRESSED *)
      ELSE  (* NOT KEYPRESSED *)
         IF (AUSG_ZIEL = 'D') OR ((AUSG_ZIEL = 'P') AND
                 NOT AKT_DRUCK_PARM.DRUCKSTOP) THEN
            LINE_ZEIGER:= 1;
      IF (AUSG_ZIEL = 'P') AND NOT ADVANCED THEN
      BEGIN
         IF AKT_DRUCK_PARM.FORMFEED THEN
            WRITE(LST,PR.FORMFEED)
         ELSE
            FOR I:= 1 TO (AKT_DRUCK_PARM.SEITENLAENGE -
                     AKT_DRUCK_PARM.OBERER_RAND - MAX_LINE) DO
               WRITE(LST,#$0D#$0A);
         ADVANCED:= TRUE;
      END;
      IF (CONT = 0) THEN
         IF (SORTIERT AND SORTEOS) OR ((NOT SORTIERT) AND (NEWTIT = NIL)) THEN
            IF (AUSG_ZIEL = 'P') THEN
               IF ((DR_BUF[DR_PTR] = '<FF>') AND (DR_CONT = 2)) OR
                  ((DR_BUF[DR_PTR] = '') AND (DR_CONT = 1)) THEN
               BEGIN
                  AUSG_UNIT_BEREIT:= TRUE;
                  ABORTED:= FALSE;
                  DR_PTR:= 1;
                  DR_CONT:= 0;
                  EXIT;
               END;
   END
   ELSE                (* ALLES AUSGEGEBEN *)
   BEGIN
      AUSG_UNIT_BEREIT:= TRUE;
      ABORTED:= FALSE;
      DR_PTR:= 1;
   END;
END; (* PROCEDURE PRINTOUT_BUFFER *)

PROCEDURE PUT_DRUCKBUFFER(VAR DR_TXT  : ANYSTR;
                              RUECK_FLAG : INTEGER);
VAR
   TEIL_STR       : STR80;
   DR_TXT_COPY    : ANYSTR;
   KTR_ZEICH      : INTEGER;
   KTR_POS        : INTEGER;

FUNCTION BLANK_STR : BOOLEAN;
VAR
   I         : INTEGER;
   TEST_CHAR : CHAR;
BEGIN
   BLANK_STR:= TRUE;
   IF (AUSG_ZIEL = 'D') AND WORDSTAR THEN
      TEST_CHAR:= #$A0
   ELSE
      TEST_CHAR:= ' ';
   FOR I:= 1 TO LENGTH(DR_BUF[DR_PTR]) DO
      IF DR_BUF[DR_PTR][I] <> TEST_CHAR  THEN
      BEGIN
         BLANK_STR:= FALSE;
         EXIT;
      END;
END;

PROCEDURE SEARCH_FIRST(VAR KTR_ZEICH : INTEGER;
                       VAR KTR_POS   : INTEGER);
VAR
  P_Z  : ARRAY[1..3] OF INTEGER;
  I    : INTEGER;
BEGIN
   P_Z[1]:= POS('<CR>',DR_TXT_COPY);
   P_Z[2]:= POS('<FF>',DR_TXT_COPY);
   P_Z[3]:= POS('<STOP>',DR_TXT_COPY);
   FOR I:= 1 TO 3 DO
      IF P_Z[I] = 0 THEN
         P_Z[I]:= $FF;

   KTR_ZEICH:= 1;
   KTR_POS:= P_Z[1];
   FOR I:= 2 TO 3 DO
   BEGIN
      IF P_Z[I] < KTR_POS THEN
      BEGIN
         KTR_POS:= P_Z[I];
         KTR_ZEICH:= I;
      END;
   END;
   IF KTR_POS = $FF THEN
      KTR_ZEICH:= 0;
END; (* PROCEDURE SEARCH_FIRST *)


PROCEDURE DO_EINRUECK;
VAR
   I : INTEGER;
BEGIN
   DR_BUF[DR_PTR]:= '';
   IF (AUSG_ZIEL = 'D') AND WORDSTAR THEN
      FOR I:= 1 TO EINRUECK DO
         INSERT(#$A0,DR_BUF[DR_PTR],1)
   ELSE
      FOR I:= 1 TO EINRUECK DO
         INSERT(' ',DR_BUF[DR_PTR],1);
END; (* PROCEDURE DO_EINRUECK *)

PROCEDURE TUE(VAR DR_TXT     : ANYSTR); (* PROCEDURE TUE (PUT_DRUCKBUFFER) *)
(************)
VAR
   BACK_COUNT,
   N_CH,
   POSI,REST_FREI  : INTEGER;
   TEMP_STR        : ANYSTR;
BEGIN
   IF (DR_BUF[DR_PTR] = '') AND (DR_PTR > 1) THEN
      DO_EINRUECK;

   POSI:= 1;      (* AN ANFANG VON DR_TXT *)
   REPEAT
      REST_FREI:= LINE_LENGTH - LENGTH(DR_BUF[DR_PTR]);
      IF REST_FREI < 0 THEN
      REST_FREI:= 0;
      N_CH:= POSI + REST_FREI;

      DR_BUF[DR_PTR]:= DR_BUF[DR_PTR] + COPY(DR_TXT,POSI,REST_FREI);

      IF LENGTH(DR_BUF[DR_PTR]) >= LINE_LENGTH THEN  (* DRUCKZEILE VOLL *)
      BEGIN
         IF ((N_CH - 1) < LENGTH(DR_TXT)) AND ((DR_TXT[N_CH] <> ' ')
             OR (DR_BUF[DR_PTR][LENGTH(DR_BUF[DR_PTR])] = ' ')) THEN
         BEGIN                        (* UMBRUCH AM ZEILENENDE ERFORDERLICH *)
            BACK_COUNT:= LENGTH(DR_BUF[DR_PTR]);

            WHILE (DR_BUF[DR_PTR][BACK_COUNT] <> ' ') AND (BACK_COUNT > 1) DO
               BACK_COUNT:= PRED(BACK_COUNT);
            IF BACK_COUNT > 1 THEN    (* UMBRUCH WIRKLICH DURCHFUEHREN *)
            BEGIN
               TEMP_STR:= COPY(DR_BUF[DR_PTR],BACK_COUNT + 1,
               LENGTH(DR_BUF[DR_PTR]) - BACK_COUNT);
                                            (* DAS SOLL AUF NAECHSTE ZEILE *)
               DR_BUF[DR_PTR][0]:= CHR(BACK_COUNT);  (* ALTE ZEILE KUERZEN *)
            END
            ELSE
               TEMP_STR:= '';
            IF (AUSG_ZIEL = 'D') AND WORDSTAR THEN
                  DR_BUF[DR_PTR]:= DR_BUF[DR_PTR] + #$8D + #$0A
            ELSE
               DR_BUF[DR_PTR]:= DR_BUF[DR_PTR] + #$0D + #$0A;

            IF NOT INC_DR_PTR_OK THEN EXIT;  (* DR_PTR INCREMENTIEREN *)
            DO_EINRUECK;
            DR_BUF[DR_PTR]:= DR_BUF[DR_PTR] + TEMP_STR;
         END
         ELSE             (* KEIN UMBRUCH ERFORDERLICH *)
         BEGIN
            IF (DR_TXT[N_CH] = ' ') AND (DR_BUF[DR_PTR][LENGTH(
                                         DR_BUF[DR_PTR])] <> ' ') THEN
            BEGIN
               POSI:= POSI + 1;(* LEERZEICHEN AM ANFANG DER ZEILE LOESCHEN *)
               DR_BUF[DR_PTR]:= DR_BUF[DR_PTR] + ' '; (* ABER HINTEN DRAN *)
            END;
         END;
      END;
      POSI:= POSI + REST_FREI;
   UNTIL POSI > LENGTH(DR_TXT);
END;  (* PROCEDURE TUE *)

BEGIN (* PROCEDURE PUT_DRUCKBUFFER *)
   IF RUECK_FLAG = 2 THEN             (* <CR> <FF> <STOP> MOEGL. *)
   BEGIN
      IF POS('<',DR_TXT) <> 0 THEN (* EVTL KONTR-ZEICHEN IM STRING *)
      BEGIN
         DR_TXT_COPY:= DR_TXT;
         REPEAT
            SEARCH_FIRST(KTR_ZEICH,KTR_POS);
            CASE KTR_ZEICH OF
               0: BEGIN    (* KEIN KONTROLLZEICHEN GEFUNDEN *)
                     IF DR_TXT_COPY <> '' THEN
                        TUE(DR_TXT_COPY);
                     IF DR_PTR >= 100 THEN EXIT;
                  END;
           1,2,3: BEGIN           (* <CR>,<FF> ODER <STOP> *)
                     TEIL_STR:= COPY(DR_TXT_COPY,1,KTR_POS - 1);
                     IF KTR_ZEICH = 3 THEN  (* <STOP> *)
                        DELETE(DR_TXT_COPY,1,KTR_POS + 5)
                     ELSE
                        DELETE(DR_TXT_COPY,1,KTR_POS + 3);

                     IF TEIL_STR <> '' THEN
                        TUE(TEIL_STR);

                     IF DR_PTR >= 100 THEN EXIT;
                     RUECK_FLAG:= 2;

                     IF (DR_BUF[DR_PTR][LENGTH(DR_BUF[DR_PTR])] <> #$0A) AND
                         (DR_BUF[DR_PTR] <> '')THEN
                     BEGIN
                        DR_BUF[DR_PTR]:= DR_BUF[DR_PTR] + #$0D + #$0A;
                        IF NOT INC_DR_PTR_OK THEN EXIT;  (* INCREMENTIEREN *)
                     END;
                     CASE KTR_ZEICH OF
                        1: BEGIN
                              DR_BUF[DR_PTR]:= '';       (* <CR> *)
                              WHILE POS('<CR>',DR_TXT_COPY) = 1 DO
                              BEGIN
                                 DELETE(DR_TXT_COPY,1,4);
                                 DR_BUF[DR_PTR]:= '<CR>';
                                 IF NOT INC_DR_PTR_OK THEN EXIT;
                              END;  (* WHILE *)
                           END;
                        2: BEGIN
                              DR_BUF[DR_PTR]:= '<FF>';   (* <FF> *)
                              IF NOT INC_DR_PTR_OK THEN EXIT;
                           END;
                        3: BEGIN
                              DR_BUF[DR_PTR]:= '<STOP>'; (* <STOP> *)
                              IF NOT INC_DR_PTR_OK THEN EXIT;
                           END;
                       END;  (* CASE *)
                     DR_BUF[DR_PTR]:= '';
                  END;
              END;    (* CASE *)
         UNTIL KTR_ZEICH = 0;
      END
      ELSE
      BEGIN
         TUE(DR_TXT);
         IF DR_PTR >= 100 THEN EXIT;
      END;
   END
   ELSE
   BEGIN
      TUE(DR_TXT);
      IF DR_PTR >= 100 THEN EXIT;
   END;
END;   (* PROCEDURE PUT_DRUCKBUFFER *)

PROCEDURE OUT_ABSTR(FIRSTREC : INTEGER); (* GIBT ABSTRACT IN DRUCKPUFFER *)
VAR
   ABSNAME : STRING[34];
   BL_POS  : INTEGER;
BEGIN
   ABSNAME:= PR.TITPFAD;
   ABSNAME:= ABSNAME + '.ABS';
   IF NOT ABS_OPEN THEN
   BEGIN
      OPENFILE(ABSF,ABSNAME,256);
      IF NOT OK THEN EXIT;
      ABS_OPEN:= TRUE;
   END;
   INIT_A_PTR;
   INPUT(FIRSTREC,79);
   IF NOT READ_OK THEN
   BEGIN
      RELEASE(A_HEAPTOP);
      EXIT;
   END;
   A_AKTPTR:= A_PREPTR^.NEXT;
   REPEAT
      IF (A_AKTPTR^.LINE[LENGTH(A_AKTPTR^.LINE)] <> ' ') AND
          (A_AKTPTR^.LINE <> '') THEN
         A_AKTPTR^.LINE:= A_AKTPTR^.LINE + ' ';
      PUT_DRUCKBUFFER(A_AKTPTR^.LINE,3);
      IF (A_AKTPTR^.RTN = #$FE) AND (A_AKTPTR^.NEXT <> NIL) THEN
         PUT_DRUCKBUFFER(CR_STR,2);
      A_AKTPTR:= A_AKTPTR^.NEXT;
   UNTIL A_AKTPTR = NIL;
   RELEASE(A_HEAPTOP);
END;

BEGIN    (* PROCEDURE AUSGABE *)
   IF (CONT = 0) AND (DR_CONT = 0) THEN    (* BUFFER LEER *)
   BEGIN
      AUSG_UNIT_BEREIT:= TRUE;
      EXIT;
   END;

   PRINTOUT_BUFFER(AUSG_UNIT_BEREIT,ABORTED);
   IF NOT AUSG_UNIT_BEREIT THEN
      EXIT;

   IF CONT = 0 THEN EXIT;
   CASE SCR_BUF[APTR].ART OF
      'Z': A:= 1;
      'B': A:= 2;
      'S': A:= 3;
    END;  (* CASE *)
   DR_CONT:= 1;
   DR_PTR:= 1;
   DR_BUF[1]:= '';
   I:= 1;
   WHILE (ITEM_TAB[A][I].C.START <> $FF) AND (I < 11) DO
   BEGIN
      CASE ITEM_TAB[A][I].C.ITEM OF
         1: DR_TXT:= SCR_BUF[APTR].S_FELD;
         2: DR_TXT:= SCR_BUF[APTR].NR;
         3: BEGIN
               DR_TXT:= SCR_BUF[APTR].TITEL1;
               DR_TXT[0]:= CHR(LENGTH(DR_TXT) - 2);
               IF DR_TXT[LENGTH(DR_TXT) + 1] = #$0D THEN
                  DR_TXT:= DR_TXT + '<CR>';
               DR_TXT:= DR_TXT + SCR_BUF[APTR].TITEL2;
            END;
         4: BEGIN
               WORKSTR:= SCR_BUF[APTR].AUTOR_A;
               WORKSTR[0]:= CHR(LENGTH(WORKSTR) - 2);
               WORKSTR:= WORKSTR + SCR_BUF[APTR].AUTOR_B;
               REPEAT
                  STRICH_POS:= POS('/',WORKSTR);
                  IF (STRICH_POS <> 0) THEN
                  BEGIN
                     WHILE (WORKSTR[STRICH_POS + 1] = #$20) DO
                        DELETE(WORKSTR,STRICH_POS + 1, 1);
                     DELETE(WORKSTR,STRICH_POS,1);
                     STRICH_POS:= STRICH_POS - 1;
                     WHILE WORKSTR[STRICH_POS] = #$20 DO
                     BEGIN
                        DELETE(WORKSTR,STRICH_POS,1);
                        STRICH_POS:= STRICH_POS - 1;
                     END;  (* WHILE *)
                     INSERT(', ',WORKSTR,STRICH_POS + 1);
                  END;
               UNTIL STRICH_POS = 0;
               DR_TXT:= WORKSTR;
            END;
         5: DR_TXT:= SCR_BUF[APTR].JAHR;
         6: DR_TXT:= SCR_BUF[APTR].SEITE;
         7: DR_TXT:= '<ABSTR>';
         8: CASE SCR_BUF[APTR].ART OF
               'Z' : DR_TXT:= SCR_BUF[APTR].ZEITSCHR;
               'B' : DR_TXT:= SCR_BUF[APTR].BVERLAG;
               'S' : DR_TXT:= SCR_BUF[APTR].HRSG;
              END;
         9: CASE SCR_BUF[APTR].ART OF
               'Z' : DR_TXT:= SCR_BUF[APTR].BAND;
               'B' : DR_TXT:= SCR_BUF[APTR].BORT;
               'S' : DR_TXT:= SCR_BUF[APTR].HRSGTITEL;
              END;
        10: DR_TXT:= SCR_BUF[APTR].SVERLAG;
        11: DR_TXT:= SCR_BUF[APTR].SORT;
       END; (* CASE *)

      IF (DR_TXT[0] <> CHR(0)) AND (DR_TXT <> '<ABSTR>') THEN  (* BESETZT *)
         PUT_DRUCKBUFFER(ITEM_TAB[A][I].PRE_STR,2);

      IF DR_TXT = '<ABSTR>' THEN    (* ABSTRACT AUSGEBEN *)
      BEGIN
         IF SCR_BUF[APTR].ABS <> 0 THEN
         BEGIN
            PUT_DRUCKBUFFER(ITEM_TAB[A][I].PRE_STR,2);
            OUT_ABSTR(SCR_BUF[APTR].ABS);
         END;
      END
      ELSE
         IF ITEM_TAB[A][I].C.ITEM = 3 THEN
            PUT_DRUCKBUFFER(DR_TXT,2) (* CR KANN VORKOMMEN *)
         ELSE
            PUT_DRUCKBUFFER(DR_TXT,3); (* 3: KEIN <CR> ODER <FF> *)

      I:= I + 1;
   END; (* WHILE *)
   CASE SCR_BUF[APTR].ART OF
      'Z': DR_TXT:= POST_STR[1];
      'B': DR_TXT:= POST_STR[2];
      'S': DR_TXT:= POST_STR[3];
     END;
   PUT_DRUCKBUFFER(DR_TXT,2);

   PUT_DRUCKBUFFER(CR_STR,2);
   AUSG_UNIT_BEREIT:= FALSE;
   IF APTR = 16 THEN
      APTR:= 1
   ELSE
      APTR:= SUCC(APTR);
   CONT:= PRED(CONT);
   IF (CONT = 0) THEN
      IF (SORTIERT AND SORTEOS) OR ((NOT SORTIERT) AND (NEWTIT = NIL)) THEN
      BEGIN
         IF AUSG_ZIEL = 'D' THEN
            PUT_DRUCKBUFFER(END_STR,2);
         IF AUSG_ZIEL = 'P' THEN
            PUT_DRUCKBUFFER(FF_STR,2);
      END;
   DR_PTR:= 1;
END;    (* PROCEDURE AUSGABE *)