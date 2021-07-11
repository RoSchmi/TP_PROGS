
(***************************************************************)
(*                    PROCEDURE LIST                           *)
(***************************************************************)
OVERLAY PROCEDURE LIST;
LABEL ESCAPE;
CONST
   TERM : CHARSET =[#$0D];
TYPE
   KEYPTR = ^KEYRECORD;
   KEYRECORD = RECORD
               KEY    : KEYNAMSTR;
               NEXT   : KEYPTR;
           END;
VAR
   HEAPTOP                        : ^INTEGER;
   FIRSTK,LASTK,
   NEWK,APTR,EPTR                 : KEYPTR;
   CONT,I                         : INTEGER;
   CO,CH,OBJ                      : CHAR;
   FIRSTKEY,SHORTKEY              : KEYNAMSTR;
   ANZZEIL                        : INTEGER;
(**)

PROCEDURE FORM (VAR ANZZEIL : INTEGER);
VAR
  EINGABE  : STRING[2];
  RESULT   : INTEGER;
BEGIN
   REPEAT
      EINGABE:='56';
      GOTOXY(1,23); WRITE('ZEILEN PRO SEITE?:'); CLREOL;
      P:= 0;
      INPUTSTR(EINGABE,2,19,22,[#$0D],TC,0,P,'.',US_T);
      VAL(EINGABE,ANZZEIL,RESULT);
   UNTIL RESULT = 0;
END;     (* END PROCEDURE FORM *)

PROCEDURE COUNT;
BEGIN
      NEWK:= APTR;
      FOR I:= 1 TO ANZZEIL DO
         NEWK:= NEWK^.NEXT;
END;
(**)

PROCEDURE INPUTKEY;
BEGIN
   IF EPTR <> NIL THEN
   BEGIN
      NEWK:= FIRSTK;
      FOR I:= 1 TO (ANZZEIL * 2) DO
      BEGIN
         NEWK^.KEY:= '';
         NEWK:= NEWK^.NEXT;
      END;
      CONT:= 0; EPTR:= FIRSTK;
      REPEAT
         NEXTKEY(NAMEINDEXFILE,DATAF,SHORTKEY);
         UNPACK(SHORTKEY,EPTR^.KEY);
         IF NOT OK THEN
         BEGIN
         EPTR^.KEY:= ''; EPTR:=NIL;
         END
         ELSE
         BEGIN
            IF ((OBJ = 'A') AND (EPTR^.KEY[1] > '#')) OR
               ((OBJ = 'Q') AND (EPTR^.KEY[1] > '$')) THEN
            BEGIN
               EPTR^.KEY:= '';
               EPTR:= NIL;
            END
            ELSE
            BEGIN
               IF (OBJ = 'A') OR (OBJ = 'Q') THEN
                  DELETE(EPTR^.KEY,1,2);
                EPTR:= EPTR^.NEXT;
                CONT:=CONT + 1;
             END;
         END;
         IF KEYPRESSED THEN
         BEGIN
            CLEARFRAME;
            READ(KBD,CH);
            IF CH = #27 THEN
               EPTR:= NIL;
         END;
      UNTIL (EPTR = NIL) OR (CONT = ANZZEIL * 2);
   END;
END;           (* END PROCEDURE INPUTKEY *)
(**)
PROCEDURE OUTPUTKEY;
BEGIN
   APTR:= FIRSTK;
   IF CO = 'P' THEN
      Y:= 1
   ELSE Y:= 4;
   WHILE (APTR^.KEY <> '') AND (Y <= MAXLINE) DO
   BEGIN
      CASE CO OF
      'P' : BEGIN
               WRITE(LST,APTR^.KEY);
               COUNT;
               IF NEWK^.KEY <> '' THEN
                  WRITE(LST,' ':33 - LENGTH(APTR^.KEY),NEWK^.KEY);
               WRITE(LST,#$0D,#$0A);
            END;
      'S' : BEGIN
               GOTOXY(1,Y);WRITE(APTR^.KEY);CLREOL;
               COUNT;
               IF NEWK^.KEY <> '' THEN
               BEGIN
                  GOTOXY(40,Y);WRITE(NEWK^.KEY);
               END;
            END;
      END;          (* END CASE *)
      Y:= Y + 1;
      APTR:= APTR^.NEXT;
   END;     (* END WHILE *)
END;        (* END PROCEDURE OUTPUTKEY *)
(********************************************************)
BEGIN        (* BEGIN PROCEDURE LIST *)
(********************************************************)
   SELECT('LIST: S)CHLUESSEL, A)UTOREN ODER Q)UELLEN ?',
      [#$0D,'S','A','Q'],OBJ,'S');
   SELECT('LIST: AUSGABE AUF  P)RINTER, S)CREEN ?',[#$0D,'P','S'],CO,'S');
   IF CO = 'P' THEN      (* IF PRINTER *)
   BEGIN
      FORM(ANZZEIL); Y:= 1; MAXLINE:=ANZZEIL;
   END
   ELSE
   BEGIN
      ANZZEIL:= 17; Y:= 4; MAXLINE:= 20;
   END;
   REPEAT
      GOTOXY(1,23); WRITE('ERSTES STICHWORT :'); CLREOL;
      FILLCHAR(FIRSTKEY,SIZEOF(FIRSTKEY),0);
      P:= 0;
      INPUTSTR(FIRSTKEY,URKEYLEN,19,22,TERM,TC,1,P,'.',US_T);
      IF OBJ = 'A' THEN
      BEGIN
        TAXKEY(FIRSTKEY,URKEYLEN - 2);
        FIRSTKEY:= PREATR + FIRSTKEY;
      END;
      IF OBJ = 'Q' THEN
      BEGIN
        TAXKEY(FIRSTKEY,URKEYLEN - 2);
        FIRSTKEY:= PREJRN + FIRSTKEY;
      END;
      IF (OBJ = 'S') AND (FIRSTKEY = '') THEN
        FIRSTKEY:= '%';
      PACK(FIRSTKEY,SHORTKEY);
      SEARCHKEY(NAMEINDEXFILE,DATAF,SHORTKEY);
      IF NOT OK THEN
      BEGIN
          GOTOXY(1,23); WRITE('KEIN STICHWORT GEFUNDEN !'); CLREOL;
          READ(KBD,CH);
          EXIT;
      END;
   UNTIL OK;
   PREVKEY(NAMEINDEXFILE,DATAF,SHORTKEY);

   IF CO = 'P' THEN                (* INITIALISIERUNG AN DRUCKER *)
      FOR I:= 1 TO LENGTH(PR.DRUCKINITSTR) DO
         CHAROUT(PR.DRUCKINITSTR[I]);

   MARK(HEAPTOP);
   NEW(FIRSTK);
   LASTK:= FIRSTK;
   FOR I:= 1 TO ((ANZZEIL * 2) - 1) DO
   BEGIN
      NEW(NEWK);
      LASTK^.NEXT:= NEWK;
      LASTK:= NEWK;
   END;
   LASTK^.NEXT:= FIRSTK;

   INPUTKEY;

   OUTPUTKEY;

   IF EPTR <> NIL THEN
   BEGIN
      REPEAT
         INPUTKEY;
         IF CO = 'S' THEN
         BEGIN
            REPEAT UNTIL KEYPRESSED;
            READ(KBD,CH);
            CLEARFRAME;
            IF CH = #27 THEN
            BEGIN
               GOTO ESCAPE;
            END;
         END
         ELSE
         BEGIN
            IF PR.FORMFEED = '1' THEN     (* AUTOSTOP *)
              BEGIN
              REPEAT UNTIL KEYPRESSED; READ(KBD,CH);
              IF CH = #27 THEN GOTO ESCAPE;
              END
            ELSE
              BEGIN
              WRITE(LST,PR.FORMFEED);    (* FORMFEED-CHR. AUSGEBEN *)
              IF KEYPRESSED THEN
                BEGIN
                READ(KBD,CH);
                IF CH = #27 THEN GOTO ESCAPE;
                END;
              END;
         END;
         OUTPUTKEY;
      UNTIL EPTR = NIL;
   END;
   REPEAT UNTIL KEYPRESSED;
   READ(KBD,CH);
   ESCAPE :
   RELEASE(HEAPTOP);
END;         (* END PROCEDUE LIST *)