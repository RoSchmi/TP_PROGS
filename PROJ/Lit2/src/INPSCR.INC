(*********************************************)
(*             ABSTR-UNTERPROGRAMME          *)
(*********************************************)


CONST
    IBM_SCREEN : BOOLEAN = TRUE;

TYPE
   LINEPOINTER = ^LINERECORD;
      LINERECORD = RECORD
         LINE        : INP_STR80;
         N1          : INTEGER;  (* DUMMY PLATZHALTER *)
         RTN         : CHAR;
         LAST,
         NEXT        : LINEPOINTER;
      END;

VAR
   A_PREPTR,
   A_AKTPTR,
   A_SCREENPTR,
   A_LASTPTR       : LINEPOINTER;
   A_HEAPTOP       : ^INTEGER;
   A_CH,A_TC       : CHAR;
   ABSREC,
   ABSREC1         : ABSTRACTREC;
   A_SCREEN        : ARRAY[0..26] OF LINEPOINTER;
   A_L,A_X,A_P,
   A_B,A_I,A_Y     : INTEGER;
   READ_OK         : BOOLEAN;

PROCEDURE ACT_SCREEN(SCR_ANF,SCR_END,Z_ANF,Z_LEN : INTEGER;
                     BLANK                       : CHAR);
VAR
   I,Y   : INTEGER;
BEGIN
   IF NOT READ_OK THEN
      EXIT;
   Y:= SCR_ANF;
   A_AKTPTR:= A_SCREENPTR;
   REPEAT
      IF A_SCREEN[Y] = NIL THEN
      BEGIN
         GOTOXY(Z_ANF + 1,Y);WRITE(A_AKTPTR^.LINE,
                   CONSTR(' ',Z_LEN - LENGTH(A_AKTPTR^.LINE)));
         WRITE(A_AKTPTR^.RTN);
         A_SCREEN[Y]:= A_AKTPTR;
         IF A_AKTPTR^.NEXT = NIL THEN
         BEGIN
            GOTOXY(Z_ANF + 1 + LENGTH(A_AKTPTR^.LINE),Y);
            WRITE(CONSTR(BLANK,Z_LEN - LENGTH(A_AKTPTR^.LINE)),#$FE);

         END;
      END;
      A_AKTPTR:= A_AKTPTR^.NEXT;
      Y:= Y + 1;
   UNTIL (A_AKTPTR = NIL) OR (Y = SCR_END + 1);

      IF (A_AKTPTR = NIL) AND (Y <> SCR_END + 1) THEN
      BEGIN
         GOTOXY(Z_ANF + 1,Y);
         IF (BLANK = ' ') AND((Z_ANF + Z_LEN) = 79) THEN
         BEGIN
            CLREOL;
            IF Y <> SCR_END THEN
            BEGIN
               GOTOXY(Z_ANF + 1,SCR_END);
               CLREOL;
            END;
         END
         ELSE
         BEGIN
            WRITE(CONSTR(BLANK,Z_LEN));
            IF Y <> SCR_END THEN
            BEGIN
               GOTOXY(Z_ANF + 1,SCR_END);
               WRITE(CONSTR(BLANK,Z_LEN));
            END;
         END;
      END;

END;

PROCEDURE SCROLL_UP(L,SCR_ANF,SCR_END,Z_ANF,Z_LEN : INTEGER);
VAR
  A,I : INTEGER;
BEGIN
   IF NOT IBM_SCREEN THEN
   BEGIN
      A:= L - 1;
      FOR I:= A TO SCR_END DO
         A_SCREEN[I]:= NIL;
   END
   ELSE
   BEGIN
      IF L < SCR_END THEN
      BEGIN
         WINDOW(Z_ANF + 1,L,Z_ANF + Z_LEN + 1,SCR_END);
         GOTOXY(1,1);
         DELLINE;
         WINDOW(1,1,80,25);
      END;
      I:= L + 1;
      WHILE I < (SCR_END + 1) DO
      BEGIN
         A_SCREEN[I - 1]:= A_SCREEN[I];
         I:= I + 1;
      END;
      A_SCREEN[SCR_END - 1]:= NIL;
      A_SCREEN[SCR_END]:= NIL;
   END;
END;
PROCEDURE SCROLL_DOWN(L,SCR_ANF,SCR_END,Z_ANF,Z_LEN : INTEGER;
                      BLANK                         : CHAR);
VAR
  A,I   : INTEGER;
BEGIN
   IF NOT IBM_SCREEN THEN
   BEGIN
      A:= L - 1;
      FOR I:= A TO SCR_END DO
         A_SCREEN[I]:= NIL;
   END
   ELSE
   BEGIN
      IF L < (SCR_END - 1) THEN
      BEGIN
         WINDOW(Z_ANF + 1,L + 1,Z_ANF + Z_LEN + 1,SCR_END);
         GOTOXY(1,1);
         INSLINE;
         GOTOXY(1,1);WRITE(CONSTR(BLANK,Z_LEN));
         WINDOW(1,1,80,25);
      END;
      A:= L + 1;
      FOR I:= SCR_END - 1 DOWNTO A DO
         A_SCREEN[I + 1]:= A_SCREEN[I];
      A_SCREEN[L]:= NIL;
      A_SCREEN[L + 1]:= NIL;
   END;
END;

PROCEDURE NEW_LINE(L,SCR_ANF,SCR_END,Z_ANF,Z_LEN : INTEGER;
                   BLANK                         : CHAR;
                   RTN                           : CHAR);
VAR
   A,I : INTEGER;
BEGIN
   IF L = SCR_END THEN
      SCROLL_UP(SCR_ANF,SCR_ANF,SCR_END,Z_ANF,Z_LEN)
   ELSE
      SCROLL_DOWN(L,SCR_ANF,SCR_END,Z_ANF,Z_LEN,BLANK);

   A_AKTPTR:= A_SCREENPTR;           (* AN ANFANG DES SCREEN *)

   FOR I:= SCR_ANF + 1 TO L DO        (* VORZAEHLEN ZUR AKT. ZEILE *)
       A_AKTPTR:= A_AKTPTR^.NEXT;

   A_LASTPTR:= A_AKTPTR;        (* NEUE ZEILE EINFUEGEN *)
   NEW(A_AKTPTR);
   A_AKTPTR^.NEXT:= A_LASTPTR^.NEXT;
   A_LASTPTR^.NEXT:= A_AKTPTR;
   A_AKTPTR^.LAST:= A_LASTPTR;
   A_LASTPTR:= A_AKTPTR^.NEXT;

   IF A_LASTPTR <> NIL THEN
      A_LASTPTR^.LAST:= A_AKTPTR;

   A_LASTPTR:= A_AKTPTR^.LAST;

   IF A_P > LENGTH(A_LASTPTR^.LINE) THEN
      A_P:= LENGTH(A_LASTPTR^.LINE);

   IF A_P < LENGTH(A_LASTPTR^.LINE) THEN
   BEGIN
  A_AKTPTR^.LINE:= COPY(A_LASTPTR^.LINE,A_P + 1,LENGTH(A_LASTPTR^.LINE) - A_P);
      A_AKTPTR^.RTN:= A_LASTPTR^.RTN;
      A_LASTPTR^.RTN:= RTN;
   END
   ELSE
   BEGIN
     A_AKTPTR^.LINE:= '';
     A_AKTPTR^.RTN:= #$FE;
   END;
   A_LASTPTR^.LINE[0]:= CHR(A_P);
   IF L = SCR_END THEN
      A_SCREENPTR:= A_SCREENPTR^.NEXT;
END;

PROCEDURE INITSCREEN(SCR_ANF,SCR_END,Z_ANF,Z_LEN : INTEGER;
                     BLANK                       : CHAR);
VAR
   I : INTEGER;
BEGIN
   IF NOT READ_OK THEN
      EXIT;
   FOR I:= SCR_ANF TO SCR_END DO
   BEGIN
      A_SCREEN[I]:= NIL;
      GOTOXY(Z_ANF + 1,I);
      IF (BLANK = ' ') AND ((Z_ANF + Z_LEN) = 79) THEN
         CLREOL
      ELSE
         WRITE(CONSTR(BLANK,Z_LEN));
   END;
 END;
(*******************************)

PROCEDURE NEU(VAR FIRSTREC,TIT_NR : INTEGER);
BEGIN
   FILLCHAR(ABSREC,SIZEOF(ABSREC),0);
   ABSREC.TIT_NR:= TIT_NR;
   ABSREC.TXT[1]:= #$1A;
   ADDREC(ABSF,FIRSTREC,ABSREC);

END;
(****************************)
PROCEDURE INIT_A_PTR;
BEGIN
   MARK(A_HEAPTOP);
   NEW(A_AKTPTR);
   A_PREPTR:= A_AKTPTR;
   A_PREPTR^.LAST:= NIL;
   A_LASTPTR:= A_PREPTR;

   NEW(A_AKTPTR);
   A_LASTPTR^.NEXT:= A_AKTPTR;
   A_AKTPTR^.LAST:= A_LASTPTR;
   A_AKTPTR^.NEXT:= NIL;
   A_AKTPTR^.LINE:= '';
   A_AKTPTR^.RTN:= #$FE;
   A_SCREENPTR:= A_PREPTR^.NEXT;
END;


PROCEDURE INPUT_REC(VAR IN_REC ;
                        REC_LEN : INTEGER;  (* BIS MAX 1024 *)
                        Z_LEN   : INTEGER;
                    VAR CNT     : INTEGER;
                    VAR INP     : INTEGER;
                    VAR TXTEND  : BOOLEAN);
TYPE
   REC = ARRAY[1..1024] OF CHAR;
VAR
   TXT    : REC ABSOLUTE IN_REC;
   BK_CNT : INTEGER;
BEGIN
      REPEAT
         REPEAT
            IF TXT[CNT] <> #$0A THEN
            BEGIN
               A_AKTPTR^.LINE[INP]:= TXT[CNT];
               INP:= INP + 1;
            END;
            CNT:= CNT + 1;
         UNTIL (TXT[CNT - 1] IN [#$0D,#$1A,#$8D]) OR
               (INP = Z_LEN + 2) OR
               (CNT >= REC_LEN);

         IF (TXT[CNT - 1] IN [#$0D,#$8D]) OR
            (INP = Z_LEN + 2) THEN    (* NEUE ZEILE *)
         BEGIN
            IF TXT[CNT - 1] = #$8D THEN
               A_AKTPTR^.RTN:= #$BA;
            IF TXT[CNT - 1] = #$0D THEN
               A_AKTPTR^.RTN:= #$FE;
            A_AKTPTR^.LINE[0]:= CHR(INP - 2);
            BK_CNT:= LENGTH(A_AKTPTR^.LINE);
          IF (INP = Z_LEN + 2) AND NOT(TXT[CNT - 1] IN [#$0D,#$8D]) THEN
            BEGIN
               BK_CNT:= LENGTH(A_AKTPTR^.LINE);
               WHILE (A_AKTPTR^.LINE[BK_CNT] <> #$20) AND (BK_CNT > 5) DO
                  BK_CNT:= BK_CNT - 1;
               IF BK_CNT = 5 THEN
                  BK_CNT:= LENGTH(A_AKTPTR^.LINE);
               CNT:= CNT - 1;
               A_AKTPTR^.RTN:= #$BA;
            END;
            A_LASTPTR:= A_AKTPTR;
            NEW(A_AKTPTR);
            A_LASTPTR^.NEXT:=A_AKTPTR;
            A_AKTPTR^.LAST:= A_LASTPTR;
            A_AKTPTR^.NEXT:= NIL;
            INP:= LENGTH(A_LASTPTR^.LINE) - BK_CNT + 1;
            A_AKTPTR^.LINE:= COPY(A_LASTPTR^.LINE,BK_CNT + 1,
                                  LENGTH(A_LASTPTR^.LINE) - BK_CNT);
            A_LASTPTR^.LINE[0]:= CHR(BK_CNT);
         END;

         IF TXT[CNT - 1] = #$1A THEN    (* TEXTENDE *)
         BEGIN
            TXTEND:= TRUE;
            A_AKTPTR^.LINE[0]:= CHR(INP - 2);
            A_AKTPTR^.RTN:= #$FE;
         END;
      UNTIL (CNT >= REC_LEN) OR TXTEND;
END;  (* PROCEDURE INPUT_REC *)

(***************************)
PROCEDURE INPUT(FIRSTREC,Z_LEN : INTEGER);
VAR
 RECNR,
 CNT,INP,
 BK_CNT     : INTEGER;
 TXTEND     : BOOLEAN;

BEGIN
   INP:= 1;
   TXTEND:= FALSE;
   RECNR:= FIRSTREC;
   READ_OK:= TRUE;

   REPEAT     (* UNTIL TXTEND *)
      IF ABSF.NUMREC > RECNR THEN
         GETREC(ABSF,RECNR,ABSREC)
      ELSE
      BEGIN
         GOTOXY(1,23);
         WRITE('FALSCHE ABSTRACT-DATEI. EINLESEN BEENDET.');CLREOL;
         READ_OK:= FALSE;
         READ(KBD,A_CH);
         EXIT;
      END;
      RECNR:= ABSREC.FOLGE;
      CNT:= 1;
      INPUT_REC(ABSREC.TXT,251,Z_LEN,CNT,INP,TXTEND);
   UNTIL TXTEND OR (RECNR = 0);
END;
(**)
PROCEDURE LOESCHE(RECNR : INTEGER);  (* LOESCHT ALLE RECORDS EINER ABSTRACT- *)
VAR                                  (* DATEI EINSCHLIESSLICH RECNR *)
   RECNRALT : INTEGER;
   ABSREC2  : ABSTRACTREC;
BEGIN
   REPEAT
      IF ABSF.NUMREC > RECNR THEN
      BEGIN
         GETREC(ABSF,RECNR,ABSREC2);
         RECNRALT:= RECNR;
         RECNR:= ABSREC2.FOLGE;
         DELETEREC(ABSF,RECNRALT);
      END
      ELSE EXIT;
   UNTIL RECNR = 0;
END;
(********************************************)
PROCEDURE OUT_DISK(FIRSTREC,TIT_NR : INTEGER);
VAR
   CNT,INP,I : INTEGER;
   RECNR     : INTEGER;
   RECNRALT  : INTEGER;
   UEBERTRAG : STRING[2];
   TXTEND    : BOOLEAN;
   NEU_REC   : BOOLEAN;

BEGIN
   IF NOT READ_OK THEN
      EXIT;
   RECNR:= FIRSTREC;
   NEU_REC:= FALSE;
   TXTEND:= FALSE;
   A_AKTPTR:= A_PREPTR^.NEXT;
   UEBERTRAG:= '';
   INP:= 1;
   REPEAT                  (* UNTIL (RECNR = 0) OR TXTEND *)
      CNT:= 1;
      REPEAT               (* UNTIL (CNT >= 251) OR TXTEND *)
      FOR I:= 1 TO LENGTH(UEBERTRAG) DO
      BEGIN
         ABSREC.TXT[CNT]:= UEBERTRAG[I];
         CNT:= CNT + 1;
      END;
         WHILE (INP <= LENGTH(A_AKTPTR^.LINE)) AND (CNT <= 250) DO
         BEGIN
            ABSREC.TXT[CNT]:= A_AKTPTR^.LINE[INP];
            CNT:= CNT + 1;
            INP:= INP + 1;
         END;   (* END WHILE *)


         IF (INP = LENGTH(A_AKTPTR^.LINE) + 1) THEN
         BEGIN
            IF (A_AKTPTR^.NEXT = NIL) THEN    (* TEXTENDE *)
            BEGIN
               IF CNT < 251 THEN
               BEGIN
                  ABSREC.TXT[CNT]:= #$1A;
                  TXTEND:= TRUE;
                  UEBERTRAG:= '';
                  CNT:= CNT + 1;
               END
               ELSE
                 UEBERTRAG:= #$1A;
            END
            ELSE
            BEGIN                            (* ZEILENENDE *)
               CASE CNT OF
               1..249 : BEGIN
                           IF A_AKTPTR^.RTN = #$BA THEN
                              ABSREC.TXT[CNT]:= #$8D
                           ELSE
                              ABSREC.TXT[CNT]:= #$0D;
                           ABSREC.TXT[CNT + 1]:= #$0A;
                           CNT:= CNT + 2;
                           UEBERTRAG:= '';
                        END;
                  250 : BEGIN
                           IF A_AKTPTR^.RTN = #$BA THEN
                              ABSREC.TXT[CNT]:= #$8D
                           ELSE
                              ABSREC.TXT[CNT]:= #$0D;
                           CNT:= CNT + 1;
                           UEBERTRAG:= #$0A;
                        END;
                  251 : BEGIN
                           IF A_AKTPTR^.RTN = #$BA THEN
                              UEBERTRAG:= #$8D
                           ELSE
                              UEBERTRAG:= #$0D;
                           UEBERTRAG:= UEBERTRAG + #$0A;
                        END;
                     END;   (* END CASE *)
            END;
            IF A_AKTPTR^.NEXT <> NIL THEN
            BEGIN
               A_AKTPTR:= A_AKTPTR^.NEXT;
               INP:= 1;
            END;
         END;    (* END IF INP = LENGTH(A_AKTPTR^.LINE) + 1 *)
      UNTIL (CNT >= 251) OR TXTEND;

      IF NOT NEU_REC THEN
         GETREC(ABSF,RECNR,ABSREC1)
      ELSE
         ABSREC1.FOLGE:= 0;
         ABSREC1.TIT_NR:= TIT_NR;

      IF (ABSREC1.FOLGE <> 0) AND (NOT TXTEND) THEN
      BEGIN
         ABSREC.FOLGE:= ABSREC1.FOLGE;
         ABSREC.TIT_NR:= TIT_NR;
         RECNRALT:= RECNR;
         RECNR:= ABSREC1.FOLGE;
         NEU_REC:= FALSE;
      END;

      IF (ABSREC1.FOLGE = 0) AND (NOT TXTEND) THEN
      BEGIN
         NEWREC(ABSF,ABSREC.FOLGE);
         RECNRALT:= RECNR;
         RECNR:= ABSREC.FOLGE;
         NEU_REC:= TRUE;
      END;

      IF TXTEND THEN
      BEGIN
         IF ABSREC1.FOLGE <> 0 THEN
            LOESCHE(ABSREC1.FOLGE);
         ABSREC.FOLGE:= 0;
         ABSREC.TIT_NR:= TIT_NR;
         RECNRALT:= RECNR;
         RECNR:= 0;
      END;
      PUTREC(ABSF,RECNRALT,ABSREC);

   (****************************)
      AKTFREEGROUPS;
      Z25;                       (* UEBERPRUEFUNG OB NOCH FREIER PLATZ *)
      CHECKFREEGROUPS;           (* AUF DISKETTE *)
      IF NOT ENOUGH THEN
      BEGIN
         OK_1:= FALSE;
         TXTEND:= TRUE;
         ABSREC.FOLGE:= 0;
         ABSREC.TIT_NR:= TIT_NR;
         PUTREC(ABSF,RECNRALT,ABSREC);
      END;
    (****************************)

   UNTIL (RECNR = 0) OR TXTEND;
END;

(**********************************)
OVERLAY PROCEDURE INPUTSCREEN(SCR_ANF,SCR_END,Z_ANF,Z_LEN,SWITCH : INTEGER;
                      BLANK                              : CHAR;
                      VAR A_TC                           : CHAR;
                      A_WINDOW                           : BOOLEAN);
CONST
   TERM : INP_CHARSET = [#$1A,#$19,#$18,#$17,#$14,#$16,#$12,#$0E,#$0D,#$8D,
                     #$08,#$05,#$03,#$02,#$0C];
VAR
   TC,RET_CH  : CHAR;
   I,A,B      : INTEGER;
   SCR_LENGTH : INTEGER;

PROCEDURE DEL_PTR(SCR_ANF,SCR_END : INTEGER);
BEGIN
   IF A_AKTPTR^.NEXT <> NIL THEN
   BEGIN
      IF A_L = SCR_ANF THEN
         A_SCREENPTR:= A_SCREENPTR^.NEXT;
      A_AKTPTR^.LAST^.NEXT:= A_AKTPTR^.NEXT;
      A_AKTPTR^.NEXT^.LAST:= A_AKTPTR^.LAST;
   END
   ELSE
      A_AKTPTR^.LAST^.NEXT:= NIL;
END;

BEGIN            (* BEGIN PROCEDURE INPUTSCREEN *)
   IF NOT READ_OK THEN
   EXIT;
   SCR_LENGTH:= SCR_END - SCR_ANF;
   A_L:= SCR_ANF;
   A_P:= 0;
   A_TC:= #$00;
   REPEAT
      A_Y:= A_L - 1;
   INPUTSTR(A_SCREEN[A_L]^.LINE,Z_LEN,Z_ANF,A_Y,TERM,TC,SWITCH,A_P,BLANK,US_T);
     CASE TC OF
 #$14 :  BEGIN                      (* AN ANFANG DES TEXTES *)
            A_SCREENPTR:= A_PREPTR^.NEXT;
            INITSCREEN(SCR_ANF,SCR_END,Z_ANF,Z_LEN,BLANK);
            ACT_SCREEN(SCR_ANF,SCR_END,Z_ANF,Z_LEN,BLANK);
         END;
 #$16 :  BEGIN                      (* AN ENDE DES TEXTES *)
            WHILE A_SCREENPTR^.NEXT <> NIL DO
               A_SCREENPTR:= A_SCREENPTR^.NEXT;
            A_AKTPTR:= A_SCREENPTR;
            I:= 1;
         WHILE (A_SCREENPTR^.LAST <> A_PREPTR) AND (I <= SCR_LENGTH) DO
            BEGIN
               A_SCREENPTR:= A_SCREENPTR^.LAST;
               I:= I + 1;
            END;
            A_L:= I + SCR_ANF - 1;
            A_P:= LENGTH(A_AKTPTR^.LINE);
            INITSCREEN(SCR_ANF,SCR_END,Z_ANF,Z_LEN,BLANK);
            ACT_SCREEN(SCR_ANF,SCR_END,Z_ANF,Z_LEN,BLANK);
         END;
 #$03 :  IF A_WINDOW THEN          (* PAGE DOWN *)
            BEEP
         ELSE
         BEGIN
            I:= 1;
            WHILE (A_SCREENPTR^.NEXT <> NIL) AND (I <= SCR_LENGTH) DO
            BEGIN
               A_SCREENPTR:= A_SCREENPTR^.NEXT;
               I:= I + 1;
            END;

            IF A_SCREENPTR^.NEXT <> NIL THEN
            BEGIN
               A_AKTPTR:= A_SCREENPTR;
               I:= SCR_ANF;
               WHILE (A_AKTPTR^.NEXT <> NIL) AND (I <= SCR_END) DO
               BEGIN
                  A_AKTPTR:= A_AKTPTR^.NEXT;
                  I:= I + 1;
               END;
               IF I <= SCR_END THEN
               BEGIN
                  A_L:= I;
                  A_P:= LENGTH(A_AKTPTR^.LINE);
               END;
            END
            ELSE
            BEGIN
               A_L:= SCR_ANF;
               A_P:= LENGTH(A_SCREENPTR^.LINE);
            END;
            INITSCREEN(SCR_ANF,SCR_END,Z_ANF,Z_LEN,BLANK);
            ACT_SCREEN(SCR_ANF,SCR_END,Z_ANF,Z_LEN,BLANK);
         END;
 #$12 :  IF A_WINDOW THEN                        (* PAGE UP *)
            BEEP
         ELSE
         IF A_SCREENPTR <> A_PREPTR^.NEXT THEN
         BEGIN
            I:= SCR_ANF;
            WHILE (A_SCREENPTR^.LAST <> A_PREPTR) AND (I <= (SCR_END - 1)) DO
            BEGIN
               A_SCREENPTR:= A_SCREENPTR^.LAST;
               I:= I + 1;
            END;
            INITSCREEN(SCR_ANF,SCR_END,Z_ANF,Z_LEN,BLANK);
            ACT_SCREEN(SCR_ANF,SCR_END,Z_ANF,Z_LEN,BLANK);
         END;

 #$08 :  BEGIN                      (* DELETE LEFT CHARACTER *)
            IF (A_P = -1) AND (A_SCREEN[A_L] <> A_PREPTR^.NEXT) THEN
            BEGIN
               A_AKTPTR:= A_SCREENPTR;
               FOR I:= SCR_ANF + 1 TO A_L DO
                  A_AKTPTR:= A_AKTPTR^.NEXT;
               A_LASTPTR:= A_AKTPTR^.LAST;
        IF (LENGTH(A_LASTPTR^.LINE) + LENGTH(A_AKTPTR^.LINE)) <= Z_LEN THEN
               BEGIN
                  SCROLL_UP(A_L,SCR_ANF,SCR_END,Z_ANF,Z_LEN);
                  A_P:= LENGTH(A_LASTPTR^.LINE);
                  A_LASTPTR^.LINE:= A_LASTPTR^.LINE + A_AKTPTR^.LINE;
                  A_LASTPTR^.RTN:= A_AKTPTR^.RTN;
                  DEL_PTR(SCR_ANF,SCR_END);
               END
               ELSE
               BEGIN
                A_P:= LENGTH(A_LASTPTR^.LINE);
                B:= Z_LEN - A_P;
                A_LASTPTR^.LINE:= A_LASTPTR^.LINE + COPY(A_AKTPTR^.LINE,1,B);
         A_AKTPTR^.LINE:= COPY(A_AKTPTR^.LINE,B + 1,LENGTH(A_AKTPTR^.LINE)-B);
                A_LASTPTR^.RTN:= #$BA;
               END;
               A_L:= A_L - 1;
               A_SCREEN[A_L]:= NIL;
               A_SCREEN[A_L + 1]:= NIL;
               ACT_SCREEN(SCR_ANF,SCR_END,Z_ANF,Z_LEN,BLANK);
            END;
         END;
 #$19 :  BEGIN                      (* DELETE LINE *)
            SCROLL_UP(A_L,SCR_ANF,SCR_END,Z_ANF,Z_LEN);
            A_AKTPTR:= A_SCREENPTR;
            IF A_L <> SCR_ANF THEN
               FOR I:= SCR_ANF + 1 TO A_L DO
                  A_AKTPTR:= A_AKTPTR^.NEXT;

            DEL_PTR(SCR_ANF,SCR_END);
            IF A_L = SCR_ANF THEN
               A_AKTPTR^.LINE:= '';
            IF (A_AKTPTR^.NEXT = NIL) AND (A_L <> SCR_ANF) THEN
               A_L:= A_L - 1;
            A_P:= 0;
            ACT_SCREEN(SCR_ANF,SCR_END,Z_ANF,Z_LEN,BLANK);
         END;
 #$0E :  BEGIN              (* INSERT LINE *)
            IF A_L = SCR_END THEN
            BEGIN
               A_P:= LENGTH(A_SCREEN[A_L - 1]^.LINE);
               NEW_LINE(A_L - 1,SCR_ANF,SCR_END,Z_ANF,Z_LEN,BLANK,#$FE);
            END
            ELSE
            BEGIN
               A_P:= 0;
               NEW_LINE(A_L,SCR_ANF,SCR_END,Z_ANF,Z_LEN,BLANK,#$FE);
            END;
            ACT_SCREEN(SCR_ANF,SCR_END,Z_ANF,Z_LEN,BLANK);
            A_P:= 0;
         END;
 #$8D,
 #$0D :  BEGIN              (* CARRIAGE RETURN *)
            IF A_WINDOW AND ((A_L = SCR_END) OR ((TC = #$0D) AND
                            (A_P = 0) AND (SCR_ANF + 1 = SCR_END))) THEN
            BEGIN
               A_TC:= #$0D;
               TC:= #$02;
            END
            ELSE
            BEGIN
               IF TC = #$8D THEN
                  RET_CH:= #$BA
               ELSE
                  RET_CH:= #$FE;
               NEW_LINE(A_L,SCR_ANF,SCR_END,Z_ANF,Z_LEN,BLANK,RET_CH);
               A_P:= LENGTH(A_AKTPTR^.LINE);
               ACT_SCREEN(SCR_ANF,SCR_END,Z_ANF,Z_LEN,BLANK);
               IF A_L = SCR_END THEN
                  A_L:= SCR_END - 1;
               A_L:= A_L + 1;
            END;
         END;

 #$05 :  BEGIN              (* LINE UP *)
            IF A_WINDOW AND (A_L = SCR_ANF) THEN
            BEGIN
               A_TC:= #$05;
               TC:= #$02;
            END
            ELSE
            IF A_L = SCR_ANF THEN
            BEGIN
               IF A_SCREENPTR = A_PREPTR^.NEXT THEN
                  BEEP
               ELSE
               BEGIN
                  A_SCREENPTR:= A_SCREENPTR^.LAST;
                  SCROLL_DOWN(SCR_ANF - 1,SCR_ANF,SCR_END,Z_ANF,Z_LEN,BLANK);
                  ACT_SCREEN(SCR_ANF,SCR_END,Z_ANF,Z_LEN,BLANK);
               END;
            END
               ELSE A_L:= A_L - 1;
         END;
 #$17 :  BEGIN                (* SCROLL UP *)
            IF A_WINDOW THEN
               BEEP
            ELSE
               IF A_SCREEN[SCR_END] <> NIL THEN
                  IF A_SCREEN[SCR_END]^.NEXT <> NIL THEN
                  BEGIN
                     A_SCREENPTR:= A_SCREEN[SCR_ANF + 1];
                     SCROLL_UP(SCR_ANF,SCR_ANF,SCR_END,Z_ANF,Z_LEN);
                     ACT_SCREEN(SCR_ANF,SCR_END,Z_ANF,Z_LEN,BLANK);
                     IF A_L >= SCR_ANF + 1 THEN
                        A_L:= A_L - 1;
                  END
                  ELSE BEEP;
         END;
 #$1A :  BEGIN                (* SCROLL DOWN *)
            IF A_WINDOW THEN
               BEEP
            ELSE
               IF A_SCREENPTR <> A_PREPTR^.NEXT THEN
               BEGIN
                  SCROLL_DOWN(SCR_ANF,SCR_ANF,SCR_END,Z_ANF,Z_LEN,BLANK);
                  A_SCREENPTR:= A_SCREENPTR^.LAST;
                  ACT_SCREEN(SCR_ANF,SCR_END,Z_ANF,Z_LEN,BLANK);
               END
               ELSE BEEP;
         END;
 #$18 :  BEGIN                (* LINE DOWN *)
          IF A_WINDOW AND ((A_L = SCR_END) OR (A_SCREEN[A_L]^.NEXT = NIL)) THEN
            BEGIN
               TC:= #$02;
               A_TC:= #$18;
            END
            ELSE
               IF A_SCREEN[A_L]^.NEXT = NIL THEN
                  BEEP
               ELSE
                  IF (A_L >= SCR_ANF) AND (A_L < SCR_END) THEN
                     A_L:= A_L + 1
                  ELSE
                  BEGIN
                     A_SCREENPTR:= A_SCREEN[SCR_ANF + 1];
                     SCROLL_UP(SCR_ANF,SCR_ANF,SCR_END,Z_ANF,Z_LEN);
                     ACT_SCREEN(SCR_ANF,SCR_END,Z_ANF,Z_LEN,BLANK);
                  END;
         END;  (* END #$18 *)
 #$0C :  A_TC:= #$0C;
 #$02 :  A_TC:= #$02;                 (* EDITIEREN BEENDEN "END" *)

              END;     (* END CASE *)

   UNTIL (TC = #$02) OR (TC = #$0C);
END;      (* PROCEDURE INPUTSCREEN *)

