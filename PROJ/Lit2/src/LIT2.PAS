PROGRAM LITERAT;
(* Compile with Turbo Pascal Vers. 3.0 *)
(*$A+,C-,R-,V-*)
(**)
CONST
   LITRECSIZE     = 256;      (* GR|ESSE EINES LITERATUR EINTRAGES *)
   URKEYLEN       = 25;       (* LAENGE EINES SCHLUESSELWORTES *)
   JRNLEN         = 30;       (* LAENGE EINES JOURNALNAMENS *)
   ATRLEN         = 20;       (* LAENGE EINES AUTORENNAMENS *)
   ANZKEY         = 12;       (* ANZAHL DER MOEGL. SCHLUESSEL PRO EINTRAG *)
   ANZKEY_PLUS_1  = 13;       (* ANZKEY + 1 *)
   ANZATR         = 15;       (* ANZAHL DER MOEGL. AUTOREN PRO EINTRAG *)
   ANZJRN         =  1;       (* ANZAHL DER MOEGL. JOURNALS PRO EINTRAG *)
   ANZGES         = 28;       (* ANZKEY + ANZATR + ANZJRN *)
   ANZVER         = 10;       (* LAENGE EINES ABSCHNITTS IN VERWEIS-DATEI *)
   PREATR         = '#.';
   PREJRN         = '$.';
   (* TURBO FILE-CONSTANTS *)
   MAXDATARECSIZE = LITRECSIZE;
   MAXKEYLEN      =  19;   (* MAXKEYLEN = (URKEYLEN * 6) DIV 8 + 1 *)
   PAGESIZE       =  16;
   ORDER          =   8;
   PAGESTACKSIZE  =  30;
   MAXHEIGHT      =   7;
VAR
  NOOFRECS    :   INTEGER;
(*$IACCESS3.BOX*)
(*$IGETKEY.BOX*)
(*$IADDKEY.BOX*)
(*$IDELKEY.BOX*)
(*$ISORT.BOX*)
(*$A+,C-,R-,V-*)
TYPE
 CHAR80ARR =  ARRAY[1..80] OF CHAR;
 MOEGL      =  0..99;
 DATEILISTE =  SET OF MOEGL;
 KEYNAMSTR  =  STRING[URKEYLEN];
 JRNSTR     =  STRING[JRNLEN];
 ATRSTR     =  STRING[ATRLEN];
   STR238   =  STRING[238];
   STR130   =  STRING[130];
   STR80    =  STRING[80];
   STR30    =  STRING[30];
   STR25    =  STRING[25];
   STR20    =  STRING[20];
   STR14    =  STRING[14];
   STR12    =  STRING[12];
   STR11    =  STRING[11];
   STR2     =  STRING[2];
   STR1     =  STRING[1];
   STR3     =  STRING[3];
   STR4     =  STRING[4];
   STR5     =  STRING[5];
   ANYSTR   =  STRING[255];
   (* CHARACTER SET TYPE *)
   CHARSET  =  SET OF CHAR;
   KW = ARRAY[1..ANZGES] OF KEYNAMSTR;
   KZ = ARRAY[1..ANZKEY] OF INTEGER;
   SCREENREC  = RECORD
      ABS         :  INTEGER;
      S_FELD      :  STRING[4];
      NR          :  STRING[5];
      TITEL1      :  STRING[74];
      TITEL2      :  STRING[74];
      AUTOR_A     :  STRING[72];
      AUTOR_B     :  STRING[72];
      ZEITSCHR    :  STRING[50];
      BVERLAG,
      HRSG,
      HRSGTITEL   :  STRING[60];
      BAND        :  STRING[20];
      BORT,
      SVERLAG,
      SORT        :  STRING[60];
      JAHR        :  STRING[12];
      SEITE       :  STRING[12];
      REC_NR      :  INTEGER;
      ART         :  CHAR;
   END;
   SCREENRECEXT = RECORD
      C           :  SCREENREC;
      S           :  KW;
   END;

   DATARECORD = RECORD
      STATUS      :  INTEGER;
      FOLGE       :  INTEGER;
      ART         :  CHAR;
      ABS         :  INTEGER;
      NR          :  INTEGER;
      S_FELD      :  STRING[4];
      LAENGE      :  INTEGER;
      INHALT      :  ARRAY[1..216] OF CHAR;
      Z           :  KZ;
   END;

   DATVERREC   =  ARRAY[1..ANZVER] OF INTEGER;
   DATVERDISK  =  ARRAY[1..ANZVER] OF BYTE;

   VERREC  = RECORD
      VERSTATUS : INTEGER;
      NUMMER    : INTEGER;
      DVR       : DATVERREC;
      DVD       : DATVERDISK;
      FOLGE     : INTEGER;
   END;

   KEYREC  = RECORD
      KEYSTATUS   :  INTEGER;
      KEYCODE     :  STRING[2];
      KEYWORD     :  STRING[URKEYLEN];
    END;

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
   DATMAP = RECORD
      RESTBYTES,
      RECSAKT,
      RECSINTL,
      MAXBED,BED      : ARRAY[1..7] OF INTEGER;
   END;
   PARAM = RECORD       (* DRUCKPARAMETER *)
      EINRUECK,
      SEITENLAENGE,
      OBERER_RAND,
      ZEILENZAHL,
      LINKER_RAND,
      RECHTER_RAND    : INTEGER;
      DRUCKSTOP,
      FORMFEED        : BOOLEAN;
   END;
   FORMAT_BUFFER = RECORD
      FORM_TEXT  : ARRAY[1..238] OF BYTE;
      P          : PARAM;
   END;

   ABSTRACTREC = RECORD
      STATUS     : INTEGER;
      TIT_NR     : INTEGER;
      FOLGE      : INTEGER;
      TXT        : ARRAY[1..252] OF CHAR;
   END;
(**)
   BDOSPARM = RECORD
      AX,BX,CX,DX,BP,SI,DI,DS,ES,FLAGS : INTEGER;
   END;
(**)
CONST
   A_INIT_1_A = '<NR> <TITEL>'; (* GEMEINSAM FUER 1. DRUCKPARAMETER *)

   Z_INIT_2_A = '<NR> <AUTOR> (<JAHR>) <TITEL> <ZEITSCHR> <BAND>: <SEITE><CR>';
   Z_INIT_2_B = '';
   Z_INIT_2_C = '';

   B_INIT_2_A = '<NR> <AUTOR> (<JAHR>) <TITEL> <BVERLAG>, <BORT>, <SEITE><CR>';
   B_INIT_2_B = '';
   B_INIT_2_C = '';

   S_INIT_2_A = '<NR> <AUTOR> (<JAHR>) <TITEL> In: <HRSG> (Hrsg.) ';
   S_INIT_2_B = '<HRSGTITEL> <SVERLAG>, <SORT>, <SEITE><CR>';
   S_INIT_2_C = '';

   PARAM_INIT : PARAM = (EINRUECK     : 6;
                         SEITENLAENGE : 72;
                         OBERER_RAND  : 4;
                         ZEILENZAHL   : 64;
                         LINKER_RAND  : 6;
                         RECHTER_RAND : 75;
                         DRUCKSTOP    : TRUE;
                         FORMFEED     : FALSE);

   PSTR : ARRAY[1..6] OF STR12 = ('TITEL','VERWEIS','SCHLUESSEL','CODE-INDEX',
          'NAME-INDEX','NUMMER-INDEX');
   DATEXT : ARRAY[1..6] OF STR4 = ('.DTA','.VER','.KEY','.IXC','.IXN','.IXZ');
   ABS_OPEN  : BOOLEAN = FALSE;
   PARM_OPEN : BOOLEAN = FALSE;
VAR
   TARECBUF_2            : ARRAY[1..5] OF INTEGER ABSOLUTE TARECBUF;
   TITEL_ANZ,
   KEY_ANZ               : INTEGER;
   FOLGE_1               : INTEGER;
   REC_ALT,REC_NEU       : INTEGER;
   DATREC                : DATARECORD;
   PR                    : PARMRECORD;
   SCR                   : SCREENRECEXT;
   KEY                   : KEYREC;
   MAXLINE,DATAF         : INTEGER;
   VERWEIS,VERWEIS1      : VERREC;
   CH,TC,AKTDRV,LOGDRV,
   TITELDRV,ART,ABO_CH   : CHAR;
   PARM                  : BDOSPARM;
   CELL,DISKNR           : BYTE;
   ERFOLG,ENOUGH,OK_1    : BOOLEAN;
   VERFILE,ABSF,
   DATF,KEYF,PARMF       : DATAFILE;
   NUMINDEXFILE,
   CODEINDEXFILE,
   NAMEINDEXFILE         : INDEXFILE;
   ERG,Y,I,LASTA,P       : INTEGER;
   RESULT                : INTEGER;
   DMASEG                : INTEGER;
   DMAOFS                : INTEGER;
   AKTDRVNR,LOGDRVNR     : INTEGER;
   DISKNUMMER            : STR2;
   TITELDRVNR            : INTEGER;
   GRUAKT,GRUTIT         : INTEGER;
   BYTGRUAKT,BYTGRUTIT   : INTEGER;
   DM                    : DATMAP;
   DRUCKPOINTER          : INTEGER;
   DTA                   : ARRAY[1..43] OF BYTE;
   DTASEG,DTAOFS,
   SETDTASEG,SETDTAOFS,
   FEHLER                : INTEGER;
   NAMR                  : STR80;
   MASK                  : CHAR80ARR;
   ALL_OPEN              : BOOLEAN;
   DAT_OPEN              : ARRAY[1..6] OF BOOLEAN;
   G_BUFFER              : ARRAY[1..256] OF BYTE;
   FORM_BUF              : FORMAT_BUFFER ABSOLUTE G_BUFFER;
   AKT_AUSG              : ARRAY[1..3] OF STR238;
   AKT_DRUCK_PARM        : PARAM;
   AKT_FORMAT_NR         : INTEGER;
   TEXTF                 : TEXT;
(* GLOBALE TYPEN UND VARIABLEN FUER SUCHE.INC *)
TYPE
   TITPTR = ^TITREC;
   TITREC = RECORD
            NR      : INTEGER;
            DISK    : BYTE;
            NEXT    : TITPTR;
   END;
   DISKPTR = ^DISKREC;
   DISKREC = RECORD
             NR    : STR2;
             NEXT  : DISKPTR;
   END;

   SCR_BUFFER = ARRAY[1..16] OF SCREENREC;

   CO = RECORD          (* 1 = <SORT-FELD> *)
      ITEM   : BYTE;    (* 2 = <NR>, 3 = <TITEL>, 4 = <AUTOR>, 5 = <JAHR>,  *)
                        (* 6 = <SEITE>, 7 = <ABSTR>, WEITER UNTERSCHIEDLICH *)
                        (* ZEITSCHR: 8 = <ZEITSCHR>, 9 = <BAND>             *)
                        (* BUCH: 8 = <BVERLAG>, 9 = <BORT>                  *)
                        (* SAMMELBAND: 8 = <HRSG>, 9 = <HRSGTITEL>,         *)
                        (*            10 = <SVERLAG> 11 = <SORT>             *)

      START  : BYTE;    (* BEGIN DES ITEMS IM STRING "AUSG" *)
      LAENGE : BYTE;    (* LAENGE DES ITEMS *)
   END;
   ZEILE = RECORD
      C         : CO;
      PRE_STR   : STRING[80]; (* ZUR AUFNAHME DES TEXTES ZWISCHEN DEN ITEMS *)
   END;

   ITEM_TABELLE  = ARRAY[1..11] OF ZEILE;
   ITEM_TABELLEN = ARRAY[1..3]  OF ITEM_TABELLE;

   POST_STRINGS  = ARRAY[1..3] OF STRING[80]; (* FUER TEXT NACH LETZTEM ITEM *)

VAR
   SCR_BUF                    : SCR_BUFFER;
   FIRSTDISK,NEWDISK,LASTDISK : DISKPTR;
   FIRSTTIT,LASTTIT,NEWTIT    : TITPTR;
   FIRSTTIT2,LASTTIT2,NEWTIT2 : TITPTR;
   ITEM_TAB                   : ITEM_TABELLEN;
   POST_STR                   : POST_STRINGS;
   AUSG_ZIEL                  : CHAR;
   APTR,EPTR,CONT             : INTEGER;
   SORTIERT,WORDSTAR          : BOOLEAN;
   SORT_ITEM,
   SORT_RESULT                : INTEGER;
   AUSG_UNIT_BEREIT           : BOOLEAN;
   ABORTED                    : BOOLEAN;
   DR_TXT                     : ANYSTR;
   LINE_LENGTH,
   MAX_LINE,
   EINRUECK,
   LINE_ZEIGER                : INTEGER;
   ADVANCED                   : BOOLEAN;
   DR_BUF                     : ARRAY[1..100] OF STR130;
   DR_CONT,DR_PTR             : INTEGER;

(***************************************************)
PROCEDURE SET_DRUCK_PARM_1;
VAR
   I : INTEGER;
BEGIN
   FOR I:= 1 TO 3 DO
   BEGIN
      AKT_AUSG[I]:= A_INIT_1_A + #$1A;
      AKT_AUSG[I][0]:= PRED(AKT_AUSG[I][0]);
   END;
   AKT_DRUCK_PARM:= PARAM_INIT;
END;
PROCEDURE SET_DRUCK_PARM_2;
VAR
   I : INTEGER;
BEGIN
   AKT_AUSG[1]:= Z_INIT_2_A + Z_INIT_2_B + Z_INIT_2_C + #$1A;
   AKT_AUSG[2]:= B_INIT_2_A + B_INIT_2_B + B_INIT_2_C + #$1A;
   AKT_AUSG[3]:= S_INIT_2_A + S_INIT_2_B + S_INIT_2_C + #$1A;
   FOR I:= 1 TO 3 DO
      AKT_AUSG[I][0]:= PRED(AKT_AUSG[I][0]);
   AKT_DRUCK_PARM:= PARAM_INIT;
END;
PROCEDURE STR5_TO_INTEGER(    ST        : STR5;
                          VAR ZAHL      : INTEGER;
                          VAR OK        : BOOLEAN;
                          VAR ST_OUT    : STR5;
                          VAR RZAHL     : REAL);
VAR
   I,RESULT  : INTEGER;
BEGIN
   FOR I:= 1 TO 5 DO
      IF ST[I] = ' ' THEN
         ST[I]:= '0';
   VAL(ST,RZAHL,RESULT);
   STR(RZAHL:5:0,ST_OUT);
   IF (RESULT = 0) AND (RZAHL >= 1.0) AND (RZAHL <= 65534.0) THEN
   BEGIN
      IF RZAHL > 32767.0 THEN
         RZAHL:= RZAHL - 65536.0;
      IF RZAHL = - 32768.0 THEN
      BEGIN
         ZAHL:= -32767;
         ZAHL:= ZAHL - 1;
      END
      ELSE
         ZAHL:= TRUNC(RZAHL);
      OK:= TRUE;
   END
   ELSE OK:= FALSE;
END;

FUNCTION INT_TO_STR5(I_ZAHL : INTEGER) : STR5;
VAR
   RZAHL    : REAL;
   TEMP_STR : STR5;
BEGIN
   RZAHL:= I_ZAHL;
   IF I_ZAHL < 0 THEN
      RZAHL:= RZAHL + 65536.0;
   STR(RZAHL : 5 : 0,TEMP_STR);
   INT_TO_STR5:= TEMP_STR;
END;
(***************************************************)
PROCEDURE Z25;
VAR
  I : INTEGER;
BEGIN

   GOTOXY(1,25);WRITE(GRUAKT:5,' ',GRUTIT:5,'    ');
   FOR I:= 1 TO 6 DO
       WRITE(' ',DM.RESTBYTES[I]);
   IF ABS_OPEN THEN
       WRITE(' ',DM.RESTBYTES[7]);
   CLREOL;

END;

PROCEDURE GETUSEDRECS;
BEGIN
   DM.RECSAKT[1]:= FILELEN(VERFILE);
   DM.RECSAKT[2]:= FILELEN(KEYF);
   DM.RECSAKT[3]:= FILELEN(CODEINDEXFILE.DATAF);
   DM.RECSAKT[4]:= FILELEN(NAMEINDEXFILE.DATAF);
   DM.RECSAKT[5]:= FILELEN(NUMINDEXFILE.DATAF);
   DM.RECSAKT[6]:= FILELEN(DATF);
   IF ABS_OPEN THEN
      DM.RECSAKT[7]:= FILELEN(ABSF);
END;






PROCEDURE CHECKFREEGROUPS;
VAR
  GRUA,GRUT,I,Z : INTEGER;
  COUNT         : INTEGER;
  CH            : CHAR;
  ST1,ST2       : STR80;
PROCEDURE TUE(VAR GRU    : INTEGER;
                  BYTGRU : INTEGER;
                  PRMT   : STR80);
BEGIN
   WITH DM DO
   BEGIN
      IF RESTBYTES[I] < MAXBED[I] THEN
      BEGIN
         GRU:= GRU - (MAXBED[I] - RESTBYTES[I]) DIV BYTGRU;
         IF (MAXBED[I] - RESTBYTES[I]) MOD BYTGRU <> 0 THEN
            GRU:= GRU - 1;
      END;
      IF GRU <= 1 THEN
      BEGIN
         ENOUGH:= FALSE;
         GOTOXY(1,23);WRITE(PRMT);CLREOL;
         READ(KBD,CH);
      END
      ELSE ENOUGH:= TRUE;
   END; (* WITH DM DO *)
END; (* END PROCEDURE TUE *)

BEGIN  (* BEGIN PROCEDURE CHECKFREEGROUPS *)
   GRUA:= GRUAKT;
   GRUT:= GRUTIT;
   ST1:='NICHT GENUG PLATZ AUF INDEX-DISKETTE!';
   ST2:='NICHT GENUG PLATZ AUF TITEL-DISKETTE!';
   IF ABS_OPEN THEN
      Z:= 7
   ELSE Z:= 6;
   FOR I:= 1 TO Z DO
   BEGIN
      IF (I >= 6) AND (AKTDRV <> TITELDRV) THEN
         TUE(GRUT,BYTGRUTIT,ST2)
      ELSE
         TUE(GRUA,BYTGRUAKT,ST1);
      IF NOT ENOUGH THEN
         EXIT;
   END; (* END FOR I:= 1 .. *)
END; (* END PROCEDURE CHECKFREEGROUPS *)

PROCEDURE AKTFREEGROUPS;
VAR
  I,VERBRAUCH,Z  : INTEGER;

PROCEDURE TUE(VAR GRU    : INTEGER;
                  BYTGRU : INTEGER);
BEGIN
   WITH DM DO
   BEGIN
      VERBRAUCH:= (RECSAKT[I] - RECSINTL[I]) * BED[I];
      IF RESTBYTES[I] >= VERBRAUCH THEN
         RESTBYTES[I]:= RESTBYTES[I] - VERBRAUCH
      ELSE
      BEGIN
         GRU:= GRU - (VERBRAUCH - RESTBYTES[I]) DIV BYTGRU;
         RESTBYTES[I]:= (VERBRAUCH - RESTBYTES[I]) MOD BYTGRU;
         IF RESTBYTES[I] <> 0 THEN
            GRU:= GRU - 1;
            RESTBYTES[I]:= BYTGRU - RESTBYTES[I];
      END; (* END ELSE *)
      RECSINTL[I]:= RECSAKT[I];
   END; (* END WITH DM DO *)
END; (* END PROCEDURE TUE *)

BEGIN (* BEGIN PROCEDURE AKTFREEGROUPS *)
   GETUSEDRECS;
   IF ABS_OPEN THEN
      Z:= 7
   ELSE Z:= 6;
   FOR I:= 1 TO Z DO
   BEGIN
      IF (I >=6) AND (AKTDRV <> TITELDRV) THEN
         TUE(GRUTIT,BYTGRUTIT)
      ELSE
         TUE(GRUAKT,BYTGRUAKT);
   END; (* END FOR I:=1... *)
END; (* END PROCEDURE AKTFREEGROUPS *)


(*$IINPSTR.INC*)




(***************************************************)
PROCEDURE OUTCASE;
VAR
  I  : INTEGER;
  CH : CHAR;
BEGIN
GOTOXY(10,8);
WRITE(CONSTR('*',59));
GOTOXY(10,18);
WRITE(CONSTR('*',59));
FOR I:= 9 TO 17 DO
BEGIN
  GOTOXY(10,I);WRITE('*');CLREOL;
  GOTOXY(68,I);WRITE('*');
END;
END;
(******************************)
PROCEDURE CHAROUT(C : CHAR);
BEGIN
   PARM.AX:= $0500;
   PARM.DX:= ORD(C);
   MSDOS(PARM);
END;
PROCEDURE TRANS_WRITE(C : CHAR);
VAR
  I      : INTEGER;
  UMLAUT : BOOLEAN;
BEGIN
   IF C IN [#$81,#$84,#$8E,#$94,#$99,#$9A,#$E1] THEN
   BEGIN
      UMLAUT:= TRUE;
      FOR I:= 1 TO LENGTH(PR.GER_UMSCHALT) DO
         CHAROUT(PR.GER_UMSCHALT[I]);
      CASE C OF
      #$81: C:= #$7D;
      #$84: C:= #$7B;
      #$8E: C:= #$5B;
      #$94: C:= #$7C;
      #$99: C:= #$5C;
      #$9A: C:= #$5D;
      #$E1: C:= #$7E;
      END;
   END
   ELSE UMLAUT:= FALSE;
   CHAROUT(C);
   IF UMLAUT THEN
   FOR I:= 1 TO LENGTH(PR.US_UMSCHALT) DO
      CHAROUT(PR.US_UMSCHALT[I]);
END;

PROCEDURE DRUCK_NONIBM(VAR DRUCKPOINTER : INTEGER);
BEGIN
   DRUCKPOINTER:= LSTOUTPTR;
   LSTOUTPTR:= OFS(TRANS_WRITE);
END;

PROCEDURE DRUCK_IBM(VAR DRUCKPOINTER : INTEGER);
BEGIN
   LSTOUTPTR:= DRUCKPOINTER;
END;

(***************************************************)
PROCEDURE MAKEKEY(    EINGAB  : STR80;
                  VAR AUSGAB  : KEYNAMSTR;
                      SWITCH  : BYTE);
VAR
  I: INTEGER;
BEGIN
   IF EINGAB = '' THEN
   BEGIN
      AUSGAB:= '';
      EXIT;
   END;
   CASE SWITCH OF
      0: BEGIN
           TAXKEY(EINGAB,URKEYLEN);
           AUSGAB:= EINGAB;
         END;
      1: BEGIN
           TAXKEY(EINGAB,URKEYLEN - 2);
           AUSGAB:= PREATR + EINGAB;
         END;
      2: BEGIN
           TAXKEY(EINGAB,URKEYLEN - 2);
           AUSGAB:= PREJRN + EINGAB;
         END;
   END;
END;



PROCEDURE GETLOGDRV;  (* ERMITTLUNG DES AKTUELLEN LAUFWERKS *)
BEGIN
  PARM.AX := $1900;
  MSDOS(PARM);
  LOGDRVNR := PARM.AX AND $00FF;
  LOGDRV := CHR(LOGDRVNR + 65);
END;

PROCEDURE LOGINTITDRV;
BEGIN
  IF LOGDRVNR <> TITELDRVNR THEN
    BEGIN
    PARM.AX:=$0E00;
    PARM.DX:=TITELDRVNR;
    MSDOS(PARM)        (* LOG-IN TITEL DRIVE *)
    END;
  END;
PROCEDURE LOGINLOG;
BEGIN
  IF LOGDRVNR <> TITELDRVNR THEN
    BEGIN
    PARM.AX:=$0E00;
    PARM.DX:=LOGDRVNR;
    MSDOS(PARM);        (* LOG-IN DRIVE AKT. *)
    END;
END;
(**)

OVERLAY PROCEDURE GETFREEGROUPS;
VAR
  GL      : REAL;
  COUNT,I : INTEGER;

BEGIN  (* PROCEDURE GETFREEGROUPS *)
  PARM.AX:= $3600;   (* BESTIMMUNG DES FREIEN PLATZES AUF D. DISKETTEN *)
  PARM.DX:= AKTDRVNR + 1;
  MSDOS(PARM);
  GRUAKT:= PARM.BX;              (* FREIE GRUPPEN AUF AKTDRV *)
  BYTGRUAKT:= PARM.AX * PARM.CX; (* BYTES PRO GRUPPE *)
  IF AKTDRV <> TITELDRV THEN
  BEGIN
      PARM.AX:= $3600;
      PARM.DX:= TITELDRVNR + 1;
      MSDOS(PARM);
      GRUTIT:= PARM.BX;               (* FREIE GRUPPEN AUF TITELDRV *)
      BYTGRUTIT:= PARM.AX * PARM.CX;  (* BYTES PRO GRUPPE *)
   END;

  WITH DM DO
  BEGIN
        (* ERMITTLUNG DES MAXIMALEN ZUSAETZL. SPEICHERBEDARFS DER BEI *)
        (* AUFRUF VON "ADD" ODER "EDIT" ENTSTEHEN KANN                *)

     BED[1]:= SIZEOF(VERWEIS);                     (* VERFILE *)
     MAXBED[1]:= BED[1] * ANZGES;
     BED[2]:= MAXKEYLEN + 6;                       (* KEYF *)
     MAXBED[2]:= BED[2] * 2;
     BED[3]:= (2 + 5) * PAGESIZE + 3;              (* CODEINDEXFILE *)
     MAXBED[3]:= BED[3] * 6;
     BED[4]:= (MAXKEYLEN + 5) * PAGESIZE + 3;      (* NAMEINDEXFILE *)
     MAXBED[4]:= BED[4] * 6;
     BED[5]:= (3 + 5) * PAGESIZE + 3;              (* NUMINDEXFILE *)
     MAXBED[5]:= BED[5] * 6;
     BED[6]:= LITRECSIZE;                          (* DATF *)
     MAXBED[6]:= LITRECSIZE;
     BED[7]:= SIZEOF(ABSTRACTREC);                 (* ABSF *)
     MAXBED[7]:= BED[7];
        (* ABSPEICHERN WIEVIEL EINTRAEGE DIE EINZELNEN DATEI BEIM *)
        (* START DES PROGRAMMS ENTHIELTEN *)
     GETUSEDRECS;
     RECSINTL:= RECSAKT;

           (* BERECHNUNG WIEVIELE FREIE BYTES IN DER ANGEBROCHE- *)
           (* NEN GRUPPE FUER JEDE DATEI NOCH ZUR VERFUEGUNG     *)
           (* STEHEN *)
           (* GL = ANZAHL DER AUF DER DISKETTE BELEGTEN BYTES    *)
           (* ALSO GROESSE EINES EINTRAGS * ANZAHL DER EINTRAEGE *)
     IF AKTDRV <> TITELDRV THEN
          COUNT:= 5
     ELSE
        IF ABS_OPEN THEN
           COUNT:= 7
        ELSE
           COUNT:= 6;
     FOR I:= 1 TO COUNT DO
     BEGIN
        GL:= BED[I] * RECSINTL[I];
        RESTBYTES[I]:= TRUNC(GL - BYTGRUAKT * INT(GL / BYTGRUAKT));
        IF RESTBYTES[I] <> 0 THEN
           RESTBYTES[I]:= BYTGRUAKT - RESTBYTES[I];
     END;
     IF AKTDRV <> TITELDRV THEN
     BEGIN
        IF ABS_OPEN THEN
           COUNT:= 7
        ELSE COUNT:= 6;
        FOR I:= 6 TO COUNT DO
        BEGIN
           GL:= BED[I] * RECSINTL[I];
           RESTBYTES[I]:= TRUNC(GL - BYTGRUTIT * INT(GL / BYTGRUTIT));
           IF RESTBYTES[I] <> 0 THEN
              RESTBYTES[I]:= BYTGRUTIT - RESTBYTES[I];
        END;
     END;

  END;  (* END WITH DM DO *)
END;    (* END PROCEDURE GETFREEGROUPS *)

(**************************)
OVERLAY PROCEDURE CONSTRUCT;
VAR
   I          : INTEGER;
   NACHFOLGER : BOOLEAN;
   KEYS       : KZ;
   TEMPSTR    : STR5;
   RZAHL      : REAL;
   I_ZAHL     : INTEGER;
PROCEDURE VERKETTE(VAR ST : STR80);
VAR
  A,I,FREI  :  INTEGER;
  REST      :  INTEGER;
BEGIN
   FREI:= SIZEOF(DATREC.INHALT) - DATREC.LAENGE;
   IF FREI < (LENGTH(ST) + 1) THEN
   BEGIN                            (* ES MUSS NEUER RECORD ANGELEGT *)
      FOR I:= 1 TO FREI DO          (* WERDEN *)
         DATREC.INHALT[DATREC.LAENGE + I]:= ST[I - 1];
      DATREC.LAENGE:= SIZEOF(DATREC.INHALT);
      IF DATREC.FOLGE = 0 THEN
      BEGIN
         NEWREC(DATF,REC_NEU);
         DATREC.FOLGE:= REC_NEU;
         NACHFOLGER:= FALSE;
      END
      ELSE
      BEGIN
         REC_NEU:= DATREC.FOLGE;
         KEYS:= DATREC.Z;
         NACHFOLGER:= TRUE;
      END;
      PUTREC(DATF,REC_ALT,DATREC);
      REC_ALT:= REC_NEU;
      IF NACHFOLGER THEN
      BEGIN
         GETREC(DATF,REC_NEU,DATREC);
         DATREC.ART:= ART;
         DATREC.ABS:= SCR.C.ABS;
         DATREC.S_FELD:= SCR.C.S_FELD;
         IF RZAHL = - 32768.0 THEN
         BEGIN
            DATREC.NR:= -32767;
            DATREC.NR:= DATREC.NR - 1;
         END
         ELSE
            DATREC.NR:= TRUNC(RZAHL);
         DATREC.Z:= KEYS;
      END
      ELSE
         DATREC.FOLGE:= 0;
      DATREC.LAENGE:= 0;
      FILLCHAR(DATREC.INHALT,SIZEOF(DATREC.INHALT),0);
      REST:= LENGTH(ST) + 1 - FREI;
      FOR I:= 1 TO REST DO
         DATREC.INHALT[DATREC.LAENGE + I]:= ST[FREI - 1 + I];
      DATREC.LAENGE:= REST;
   END
   ELSE
   BEGIN
      A:= DATREC.LAENGE + 1;
      FOR I:= 0 TO LENGTH(ST) DO
         DATREC.INHALT[A + I]:= ST[I];
      DATREC.LAENGE:= DATREC.LAENGE + LENGTH(ST) + 1;
   END;
END;       (* END PROCEDURE VERKETTE *)
BEGIN      (* PROCEDURE CONSTRUCT *)
   DATREC.FOLGE:= FOLGE_1;
   DATREC.LAENGE:= 0;
   DATREC.ART:= ART;
   DATREC.ABS:= SCR.C.ABS;
   DATREC.S_FELD:= SCR.C.S_FELD;
   STR5_TO_INTEGER(SCR.C.NR,I_ZAHL,OK,TEMPSTR,RZAHL);
   DATREC.NR:= I_ZAHL;
   FILLCHAR(DATREC.INHALT,SIZEOF(DATREC.INHALT),0);
   VERKETTE(SCR.C.TITEL1);
   VERKETTE(SCR.C.TITEL2);
   VERKETTE(SCR.C.AUTOR_A);
   VERKETTE(SCR.C.AUTOR_B);
   CASE ART OF
      'Z' : BEGIN
               VERKETTE(SCR.C.ZEITSCHR);
               VERKETTE(SCR.C.BAND);
            END;
      'B' : BEGIN
               VERKETTE(SCR.C.BVERLAG);
               VERKETTE(SCR.C.BORT);
            END;
      'S' : BEGIN
               VERKETTE(SCR.C.HRSG);
               VERKETTE(SCR.C.HRSGTITEL);
               VERKETTE(SCR.C.SVERLAG);
               VERKETTE(SCR.C.SORT);
            END;
        END;
   VERKETTE(SCR.C.JAHR);
   VERKETTE(SCR.C.SEITE);
   PUTREC(DATF,REC_ALT,DATREC);
END;
(**************************)
OVERLAY PROCEDURE RECONSTRUCT(VAR SCR : SCREENRECEXT);
VAR
   ZEIGER,I,L  : INTEGER;
   RZAHL       : REAL;

PROCEDURE TUE(VAR ST : STR80);
VAR
   ST_LENGTH,REST,I : INTEGER;
BEGIN
   WITH DATREC DO
   BEGIN
      ST_LENGTH:= ORD(INHALT[ZEIGER]);
      REST:= SIZEOF(INHALT) - ZEIGER + 1;
      IF (ST_LENGTH + 1) > REST THEN
      BEGIN
         FOR I:= 0 TO REST - 1 DO
            ST[I]:= INHALT[ZEIGER + I];
         GETREC(DATF,FOLGE,DATREC);
         ZEIGER:= 1;
         FOR I:= REST TO ST_LENGTH DO
            ST[I]:= INHALT[ZEIGER + I - REST];
         ZEIGER:= ST_LENGTH - REST + 2;
      END
      ELSE
      BEGIN
         FOR I:= 0 TO ST_LENGTH DO
            ST[I]:= INHALT[ZEIGER + I];
         ZEIGER:= ZEIGER + ST_LENGTH + 1;
      END;
   END;   (* END WITH DATREC DO *)
END;      (* END PROCEDURE TUE *)

BEGIN     (* BEGIN PROCEDURE RECONSTRUCT *)
   ART:= DATREC.ART;
   FOLGE_1:= DATREC.FOLGE;
   FILLCHAR(SCR,SIZEOF(SCR),0);
   SCR.C.ABS:= DATREC.ABS;
   SCR.C.S_FELD:= DATREC.S_FELD;
   SCR.C.NR:= INT_TO_STR5(DATREC.NR);
   L:= 1;
   ZEIGER:= 1;
   WITH SCR DO
   REPEAT
      CASE L OF
         1 : TUE(C.TITEL1);
         2 : TUE(C.TITEL2);
         3 : TUE(C.AUTOR_A);
         4 : TUE(C.AUTOR_B);
         5 : CASE ART OF
               'Z' : TUE(C.ZEITSCHR);
               'B' : TUE(C.BVERLAG);
               'S' : TUE(C.HRSG);
             END;
         6 : IF ART = 'S' THEN
                TUE(C.HRSGTITEL);
         7 : CASE ART OF
               'Z' : TUE(C.BAND);
               'B' : TUE(C.BORT);
               'S' : TUE(C.SVERLAG);
             END;
         8 : IF ART = 'S' THEN
                TUE(C.SORT);
         9 : TUE(C.JAHR);
        10 : TUE(C.SEITE);
     END;  (* END CASE *)
     L:= L + 1;
   UNTIL L = 11;
END;   (* END PROCEDURE RECONSTRUCT *)



(***********************************************************)
(* CLEARFRAME CLEARS THE DISPLAY FRAME, I.E. LINES 3 TO 20 *)
(***********************************************************)
PROCEDURE CLEARFRAME;
VAR
  I : INTEGER;
BEGIN
   FOR I:= 3 TO 20 DO
   BEGIN
     GOTOXY(1,I + 1);CLREOL;
   END;
END;

PROCEDURE SETDTA( SEGMENT, OFFSET: INTEGER; VAR Fehler : INTEGER );
BEGIN
  PARM.AX:= $1A00;
  PARM.DS := SEGMENT;
  PARM.DX := OFFSET;
  MSDOS(PARM);
  FEHLER := PARM.AX AND $FF;
END;


PROCEDURE GETDTA( VAR SEGMENT, OFFSET : INTEGER;
                         VAR FEHLER : INTEGER );
BEGIN
  PARM.AX := $2F00;
  MSDOS(PARM);
  SEGMENT := PARM.ES;
  OFFSET := PARM.BX;
  FEHLER := PARM.AX AND $FF;
END;

PROCEDURE GETFIRST( MASK            : CHAR80ARR;
                    VAR NAMR        : STR80;
                    SEGMENT, OFFSET : INTEGER;
                    OPTION          : INTEGER;
                    VAR FEHLER      : INTEGER );
CONST
   DIRSTR : STRING[6] = ' <DIR>';
VAR
  I,J    : INTEGER;
  ATTRIB : BYTE;
BEGIN
  FEHLER := 0;
  PARM.AX := $4E00;
  PARM.DS := Seg( Mask );
  PARM.DX := Ofs( Mask );
  PARM.CX := Option;
  MSDOS(PARM);
  FEHLER := PARM.AX AND $FF;
  I := 1;
  REPEAT
    NAMR[ I ] := CHR( MEM[ SEGMENT : OFFSET + 29 + I ] );
    I := I + 1;
  UNTIL ( NOT ( NAMR[ I - 1 ] IN [ ' '..'~' ] ));
  NAMR[ 0 ] := CHR( I - 1 );
  ATTRIB:= MEM[SEGMENT : OFFSET + 21];
  IF (ATTRIB AND $10) <> 0 THEN  (* SUBDIRECTORY *)
  BEGIN
     FOR J:= 1 TO  6 DO
        NAMR[I - 1 + J]:= DIRSTR[J];
     NAMR[0]:= CHR(I + 5);
  END;
END;

PROCEDURE GETNEXTENTRY( VAR NAMR        : STR80;
                        SEGMENT, OFFSET : INTEGER;
                        OPTION          : INTEGER;
                        VAR FEHLER      : INTEGER );
CONST
   DIRSTR : STRING[6] = ' <DIR>';
VAR
  I,J    : INTEGER;
  ATTRIB : BYTE;
BEGIN
  FEHLER := 0;
  PARM.AX := $4F00;
  PARM.CX := OPTION;
  MSDOS(PARM);
  FEHLER := PARM.AX AND $FF;
  I := 1;
  REPEAT
    NAMR[ I ] := CHR( MEM[ SEGMENT : OFFSET + 29 + I ] );
    I := I + 1;
  UNTIL ( NOT ( NAMR[ I - 1 ] IN [ ' '..'~' ] ));
  NAMR[ 0 ] := Chr( I - 1 );
  ATTRIB:= MEM[SEGMENT : OFFSET + 21];
  IF (ATTRIB AND $10) <> 0 THEN  (* SUBDIRECTORY *)
  BEGIN
     FOR J:= 1 TO  6 DO
        NAMR[I - 1 + J]:= DIRSTR[J];
     NAMR[0]:= CHR(I + 5);
  END;
END;

PROCEDURE DIRECTORY(DATMASK : STR12;OPTION : INTEGER);
VAR
   BUFFER       : STRING[42];
   X,Y,I        : INTEGER;
BEGIN
   FILLCHAR(DTA,SIZEOF(DTA),0);
   FILLCHAR(MASK,SIZEOF(MASK),0);
   FILLCHAR(NAMR,SIZEOF(NAMR),0);
   GETDTA(DTASEG,DTAOFS,FEHLER);
   SETDTASEG:= SEG(DTA);
   SETDTAOFS:= OFS(DTA);
   SETDTA(SETDTASEG,SETDTAOFS,FEHLER);
   GOTOXY(1,23);
   WRITE('BITTE LAUFWERK UND PFAD EINGEBEN (DR:\PFAD):');
   BUFFER:= '';
   P:= 0;
   INPUTSTR(BUFFER,30,45,22,[#$0D],TC,7,P,'.',US_T);
   IF BUFFER[LENGTH(BUFFER)] <> '\' THEN
      BUFFER:= BUFFER + '\';
   BUFFER:= BUFFER + DATMASK;
   FOR I:= 1 TO LENGTH(BUFFER) DO
      MASK[I]:= BUFFER[I];
   CLEARFRAME;
   GETFIRST(MASK,NAMR,SETDTASEG,SETDTAOFS,OPTION,FEHLER);
   IF (FEHLER = 0) THEN
   BEGIN
      X:=1;Y:=4;
      GOTOXY(X,Y);WRITE(NAMR);
      X:= X + 16;
      REPEAT
         REPEAT
            GETNEXTENTRY(NAMR,SETDTASEG,SETDTAOFS,OPTION,FEHLER);
            IF (FEHLER = 0) THEN
            BEGIN
               GOTOXY(X,Y);WRITE(NAMR);
               X:= X + 16;
            END;
         UNTIL (X >= 66) OR (FEHLER <> 0);
         X:= 1; Y:= Y + 1;
      UNTIL (Y = 21) OR (FEHLER <> 0);
   END;
   SETDTA(DTASEG,DTAOFS,FEHLER);
END;

PROCEDURE GET_DATNAMES(L : INTEGER);
VAR
   CH,TC  : CHAR;
BEGIN
   GOTOXY(14,9);
   WRITE('LETZTE INDEXDATEIEN: ',PR.IDXPFAD);
   GOTOXY(16,10);
   WRITE('NEUE INDEXDATEIEN: ');
   GOTOXY(16,14);
   WRITE('LETZTE TITELDATEI: ',PR.TITPFAD);
   GOTOXY(18,15);
   WRITE('NEUE TITELDATEI: ');
   GOTOXY(12,17);
   CH:= 'Z';
   IF L = 1 THEN
      SELECT('ALTE DATEINAMEN UEBERNEHMEN (Y/N)?',['Y','N',#$0D],CH,'Y');

   IF (CH = 'N') OR (CH = 'Z') THEN
      BEGIN
         IF CH = 'N' THEN
         BEGIN
            PR.IDXPFAD:= '';
            PR.TITPFAD:= '';
         END;
         IF CH = 'Z' THEN
            PR.TITPFAD:= '';

         REPEAT
            P:= 0;
            CASE L OF
       1 : INPUTSTR(PR.IDXPFAD,30,34,9,[#$05,#$18,#$0D,#$1A],TC,7,P,'.',US_T);
       2 : INPUTSTR(PR.TITPFAD,30,34,14,[#$05,#$18,#$0D,#$1A],TC,7,P,'.',US_T);
            END;
            IF (TC = #$18) OR (TC = #$0D) THEN
               IF L = 2 THEN
                 L:= 1
               ELSE L:= L + 1
            ELSE
               IF (TC = #$05) THEN
                 IF L = 1 THEN
                    L:= 2
                 ELSE L:= L - 1;
         UNTIL ((TC = #$0D) AND (L = 1)) OR (TC = #$1A);
      END;
END;

PROCEDURE EROEFFNE(VAR DATEI1            : INDEXFILE;
                   VAR DATEI2            : DATAFILE;
                   SIZE,I,DATNR          : INTEGER);
VAR
  CH       : CHAR;
  NAME     : STR12;
  A        : INTEGER;
  FIRSTSTR : STRING[2];
  PFAD     : STRING[35];
BEGIN
   IF I = 1 THEN
      PFAD:= PR.TITPFAD
   ELSE
      PFAD:= PR.IDXPFAD;
   IF DATNR = 1 THEN
      OPENINDEX(DATEI1,PFAD + DATEXT[I],SIZE,0)
   ELSE
      OPENFILE(DATEI2,PFAD + DATEXT[I],SIZE);
   IF OK AND (I = 1) THEN
      TITEL_ANZ:= TARECBUF_2[5];
   IF OK AND (I = 5) THEN
      KEY_ANZ:= TARECBUF_2[5];
   IF NOT OK THEN
   BEGIN
     SELECT(CONCAT(PSTR[I],'DATEI FEHLT. CREATE (Y/N)?'),['Y','N'],CH,' ');
     IF CH = 'Y' THEN
     BEGIN
       IF DATNR = 1 THEN
       BEGIN
          IF I = 5 THEN   (* NAMEINDEXFILE *)
          BEGIN
             KEY_ANZ:= 0;
             TARECBUF_2[5]:= KEY_ANZ;
          END;
          MAKEINDEX(DATEI1,PFAD + DATEXT[I],SIZE,0);
       END
       ELSE
       BEGIN
          IF I = 1 THEN    (* TITEL-DATEI *)
          BEGIN
             TITEL_ANZ:= 0;
             TARECBUF_2[5]:= TITEL_ANZ;
          END;
          MAKEFILE(DATEI2,PFAD + DATEXT[I],SIZE);
       END;
       IF NOT OK THEN
       BEGIN
         GOTOXY(1,23);WRITE('DATEI ERZEUGUNG NICHT MOEGLICH. ');CLREOL;
         READ(KBD,CH);
         EXIT;
       END
       ELSE
       BEGIN
         GOTOXY(1,Y);WRITE(PSTR[I],'DATEI "',PFAD + DATEXT[I],'" ERZEUGT.');
         CLREOL;
         Y:= Y + 1;
         DAT_OPEN[I]:= TRUE;
       END;
     END
     ELSE
        EXIT
   END
   ELSE
   BEGIN
     GOTOXY(1,Y);
     WRITE(PSTR[I],'DATEI "',PFAD + DATEXT[I],'" EROEFFNET.');CLREOL;
     Y:= Y + 1;
     DAT_OPEN[I]:= TRUE;
   END;
END;

PROCEDURE OPEN_DATS;
(*********************************************************)
(*      EROEFFNEN ODER ERZEUGEN DER DATEIEN              *)
(*********************************************************)
              (* DIE PROZEDUR EROEFFNE HAT FOLGENDE PARAMETER: *)
              (* VAR DATEI1   : INDEXFILE  *)
              (* VAR DATEI2   : DATAFILE   *)
              (* SIZE,I,DATNR : INTEGER    *)
              (* ES IST DAHER NOETIG IMMER EINE INDEXDATEI UND *)
              (* UND EINE DATENDATEI ALS PARAMETER ZU UEBER-   *)
              (* GEBEN. WELCHE DIESER DATEIEN NUN DURCH DIE    *)
              (* PROZEDURE EROEFFNE WIRKLICH EROEFFNET WIRD,   *)
              (* WIRD DURCH DEN PARAMETER DATNR FESTGELEGT.    *)
              (* SIZE BESTIMMT DIE GROESSE EINES SATZES DER    *)
              (* DATEI. I GIBT DIE NUMMER ZUR ERMITTLUNG DES   *)
              (* DATEINAMENS AN.                               *)
BEGIN
   FOR I:= 1 TO 6 DO
      DAT_OPEN[I]:= FALSE;  (* ZUR INITIALISIERUNG DAT_OPEN ARRAY AUF FALSE *)
   ALL_OPEN:= FALSE;        (* ALL_OPEN WIRD AUF TRUE GESEZT WENN DIE LETZTE *)
                            (* DATEI ERFOLGREICH EROEFFNET WURDE *)
   EROEFFNE(CODEINDEXFILE,DATF,LITRECSIZE,1,2);
                          (***)
   IF NOT OK THEN EXIT;
   EROEFFNE(CODEINDEXFILE,VERFILE,SIZEOF(VERREC),2,2);
                         (*******)
   IF NOT OK THEN EXIT;
   EROEFFNE(CODEINDEXFILE,KEYF,MAXKEYLEN + 6,3,2);
                         (****)
   IF NOT OK THEN EXIT;
   EROEFFNE(CODEINDEXFILE,KEYF,2,4,1);
            (************)
   IF NOT OK THEN EXIT;
   EROEFFNE(NAMEINDEXFILE,KEYF,MAXKEYLEN,5,1);
            (************)
   IF NOT OK THEN EXIT;
   EROEFFNE(NUMINDEXFILE,KEYF,3,6,1);
            (***********)
   IF NOT OK THEN EXIT;
   ALL_OPEN:= TRUE;
END;
(***************************)
PROCEDURE CLOSE_DATS;
BEGIN
   IF DAT_OPEN[1] THEN
   BEGIN
      TARECBUF_2[5]:= TITEL_ANZ;
      CLOSEFILE(DATF);
      DAT_OPEN[1]:= FALSE;
   END;
   IF DAT_OPEN[2] THEN
   BEGIN
      CLOSEFILE(VERFILE);
      DAT_OPEN[2]:= FALSE;
   END;
   IF DAT_OPEN[3] THEN
   BEGIN
      CLOSEFILE(KEYF);
      DAT_OPEN[3]:= FALSE;
   END;
   IF DAT_OPEN[4] THEN
   BEGIN
      CLOSEINDEX(CODEINDEXFILE);
      DAT_OPEN[4]:= FALSE;
   END;
   IF DAT_OPEN[5] THEN
   BEGIN
      TARECBUF_2[5]:= KEY_ANZ;
      CLOSEINDEX(NAMEINDEXFILE);
      DAT_OPEN[5]:= FALSE;
   END;
   IF DAT_OPEN[6] THEN
   BEGIN
      CLOSEINDEX(NUMINDEXFILE);
      DAT_OPEN[6]:= FALSE;
   END;
   IF ABS_OPEN THEN
   BEGIN
      CLOSEFILE(ABSF);
      ABS_OPEN:= FALSE;
   END;
   IF PARM_OPEN THEN
   BEGIN
      CLOSEFILE(PARMF);
      PARM_OPEN:= FALSE;
   END;
END;

(****************************)
PROCEDURE CLEARFORM;
BEGIN
   GOTOXY(33,4);WRITE('':7);
   GOTOXY(53,4);WRITE('':4);
   GOTOXY(70,4);WRITE('':5);
   GOTOXY(8,6);CLREOL;
   GOTOXY(1,7);CLREOL;
   GOTOXY(10,9);CLREOL;
   GOTOXY(10,10);CLREOL;
   GOTOXY(14,11);CLREOL;
   GOTOXY(14,12);CLREOL;
   GOTOXY(14,13);CLREOL;
   GOTOXY(14,14);CLREOL;
   GOTOXY(14,15);CLREOL;
   GOTOXY(26,16);WRITE('':12);
   GOTOXY(48,16);WRITE('':12);
   GOTOXY(2,18);WRITE('':URKEYLEN);
   GOTOXY(28,18);WRITE('':URKEYLEN);
   GOTOXY(54,18);WRITE('':URKEYLEN);
   GOTOXY(2,19);WRITE('':URKEYLEN);
   GOTOXY(28,19);WRITE('':URKEYLEN);
   GOTOXY(54,19);WRITE('':URKEYLEN);
   GOTOXY(2,20);WRITE('':URKEYLEN);
   GOTOXY(28,20);WRITE('':URKEYLEN);
   GOTOXY(54,20);WRITE('':URKEYLEN);
   GOTOXY(2,21);WRITE('':URKEYLEN);
   GOTOXY(28,21);WRITE('':URKEYLEN);
   GOTOXY(54,21);WRITE('':URKEYLEN);
END;

(**********************************************************)

PROCEDURE PACK (    INSTR   : KEYNAMSTR;
(*************) VAR OUTSTR  : KEYNAMSTR);
TYPE
   A = ARRAY[0..15] OF BYTE;

CONST

ARR02 : A = ($20,$23,$24,$21,$22,$25,$26,$27,$28,$29,$2A,$2B,$2C,$2D,$2E,$2F);
ARR03 : A = ($30,$31,$32,$33,$34,$35,$36,$37,$38,$39,$3A,$3B,$20,$20,$20,$3C);
ARR04 : A = ($3D,$3E,$40,$41,$42,$43,$44,$45,$46,$47,$48,$49,$4A,$4B,$4C,$4D);
ARR05 : A = ($4F,$50,$51,$52,$53,$54,$56,$57,$58,$59,$5A,$5B,$5C,$5D,$5E,$5F);
ARR06 : A = ($3D,$3E,$40,$41,$42,$43,$44,$45,$46,$47,$48,$49,$4A,$4B,$4C,$4D);
ARR07 : A = ($4F,$50,$51,$52,$53,$54,$56,$57,$58,$59,$5A,$5B,$5C,$5D,$5E,$5F);
ARR08 : A = ($20,$55,$20,$20,$3F,$20,$20,$20,$20,$20,$20,$20,$20,$20,$3F,$20);
ARR09 : A = ($20,$20,$20,$20,$4E,$20,$20,$20,$20,$4E,$55,$20,$20,$20,$20,$20);

   AUSGA = 6;

VAR
   ARR      : ARRAY[0..URKEYLEN] OF BYTE ABSOLUTE OUTSTR;
   ZIEL,I,Z,
   COUNT    : INTEGER;

   GESARR   : ARRAY[$20..$9F] OF CHAR ABSOLUTE ARR02;

BEGIN  (* PROCEDURE PACK *)

   FOR I:= 1 TO LENGTH(INSTR) DO
   BEGIN
      IF INSTR[I] = #225 THEN
      BEGIN
         INSTR[I]:= 'S';            (* UMWANDLUNG VON ESZET IN SS *)
         INSERT('S',INSTR,I);
      END;
   INSTR[I]:=GESARR[ORD(INSTR[I])];
   END;
   FOR I:= 0 TO URKEYLEN DO
      ARR[I]:= 0;
   ZIEL:= AUSGA;
   Z:= 0;
   FOR I:= 1 TO LENGTH(INSTR) DO
   BEGIN
      ZIEL:= ZIEL - AUSGA;
      IF ZIEL < AUSGA THEN
         ZIEL:= ZIEL + 8;
      COUNT:= ZIEL - AUSGA;
      ARR[Z]:= ARR[Z] OR ((ORD(INSTR[I]) - $20) SHR (8 - COUNT));
      IF ZIEL >= 8 THEN
         Z:= Z + 1;
      ARR[Z]:= ARR[Z] OR ((ORD(INSTR[I]) - $20) SHL COUNT);
   END;
   Z:= LENGTH(INSTR) * AUSGA;
   ARR[0]:= Z DIV 8;
   IF Z MOD 8 <> 0 THEN
      ARR[0]:= ARR[0] + 1;
END; (* PROCEDURE PACK *)

PROCEDURE UNPACK  (VAR INSTR  : KEYNAMSTR;
(***************)  VAR OUTSTR : KEYNAMSTR);
TYPE
   A = ARRAY[0..15] OF BYTE;
CONST
   L = 6;

ARR02 : A = ($20,$23,$24,$21,$22,$25,$26,$27,$28,$29,$2A,$2B,$2C,$2D,$2E,$2F);
ARR03 : A = ($30,$31,$32,$33,$34,$35,$36,$37,$38,$39,$3A,$3B,$3F,$40,$41,$8E);
ARR04 : A = ($42,$43,$44,$45,$46,$47,$48,$49,$4A,$4B,$4C,$4D,$4E,$4F,$99,$50);
ARR05 : A = ($51,$52,$53,$54,$55,$9A,$56,$57,$58,$59,$5A,$5B,$5C,$5D,$5E,$5F);


VAR
   ARR     : ARRAY[0..URKEYLEN] OF BYTE ABSOLUTE OUTSTR;
   I,N,P,
   ANZ     : INTEGER;
   GESARR  : ARRAY[$20..$5F] OF CHAR ABSOLUTE ARR02;
BEGIN
   ANZ:=(LENGTH(INSTR) * 8) DIV L;
   FOR I:= 1 TO ANZ DO
   BEGIN
      N:=(I * L) DIV 8 + 1;    (* BYTE VON INSTR *)
      P:=8 -((I * L) MOD 8);   (* PLATZ INNERHALB DES BYTE *)
      ARR[I]:= ORD(INSTR[N]) SHR P;
      IF P > (8 - L) THEN
         ARR[I]:= ARR[I] OR (ORD(INSTR[N-1]) SHL (8 - P));
      ARR[I]:= (ARR[I] AND NOT ($FF SHL L)) + $20;
   END;
   ARR[0]:= ANZ;
   FOR I:= 1 TO LENGTH(OUTSTR) DO
      OUTSTR[I]:= GESARR[ORD(OUTSTR[I])];
END; (* PROCEDURE UNPACK *)
  (**)

PROCEDURE INP;      (* EINGABE FUER TURBO-SORT *)
VAR
   SHORT_SCR : SCREENREC ABSOLUTE SCR;
BEGIN
   NEWTIT:= FIRSTTIT;     (* AN ANFANG DER LISTE *)
   WHILE (NEWTIT <> NIL) DO
   BEGIN
      IF NEWTIT^.DISK = DISKNR THEN
      BEGIN
         GETREC(DATF,NEWTIT^.NR,DATREC);
         RECONSTRUCT(SCR);
         SCR.C.REC_NR:= NEWTIT^.NR;
         SCR.C.ART:= DATREC.ART;
         SORTRELEASE(SHORT_SCR);
      END;
      NEWTIT:= NEWTIT^.NEXT;
   END;  (* END WHILE *)
END;     (* END PROCEDURE INP *)

FUNCTION LESS;      (* VERGLEICHS-ROUTINE FUER TURBO-SORT *)
VAR
   SHORT_SCR1 : SCREENREC ABSOLUTE X;
   SHORT_SCR2 : SCREENREC ABSOLUTE Y;
BEGIN
   CASE SORT_ITEM OF
      1: LESS:= SHORT_SCR1.NR < SHORT_SCR2.NR;
      2: LESS:= SHORT_SCR1.TITEL1 < SHORT_SCR2.TITEL1;
      3: LESS:= SHORT_SCR1.AUTOR_A < SHORT_SCR2.AUTOR_A;
      4: LESS:= SHORT_SCR1.JAHR < SHORT_SCR2.JAHR;
      5: LESS:= SHORT_SCR1.S_FELD < SHORT_SCR2.S_FELD;
     END; (* CASE *)
END;   (* FUNCTION LESS *)

(*****************************************)
(*$IINPSCR.INC*)
(*$IAUSGABE.INC*)
(*$ILIST.INC*)
(*****************************************)
PROCEDURE TRENNUNG;
BEGIN
END;
(*********************************************************)
(*$IUPDATE.INC*)
(*$ISUCHE.INC*)
(*********************************************************)
PROCEDURE OUTP;      (* AUSGABE-ROUTINE VON TURBO-SORT *)
BEGIN
   REPEAT
      IF CONT < 16 THEN
         IF NOT SORTEOS THEN
         BEGIN
            SORTRETURN(SCR_BUF[EPTR]);
            IF EPTR = 16 THEN
               EPTR:= 1
            ELSE
               EPTR:= SUCC(EPTR);
            CONT:= SUCC(CONT);
         END;
      AUSGABE(AUSG_UNIT_BEREIT,ABORTED);
   UNTIL (SORTEOS AND AUSG_UNIT_BEREIT) OR ABORTED;
   IF AUSG_ZIEL = 'D' THEN
      CLOSE(TEXTF);
END;  (* PROCEDURE OUTP *)
    (*********************************)
    (* MAIN PROGRAM                  *)
    (*********************************)
BEGIN
   NORMVIDEO;
   CLRSCR;
   OUTCASE;
   GOTOXY(25,12);
   WRITE('LITERATUR-ARCHIVIERUNGSSYSTEM');
   GOTOXY(22,13);
   WRITE('COPYRIGHT (C) BY ROLAND SCHMIDT 1986');
   GOTOXY(16,15);
   WRITE('BITTE ARBEITSDISKETTEN EINLEGEN UND');
   GOTOXY(16,16);
   WRITE('BELIEBIGE TASTE DRUECKEN!');
   READ(KBD,CH);
   CLRSCR;
   WRITELN(CONSTR('-',79));
   WRITELN('LITERATUR-ARCHIVIERUNGS-SYSTEM');
   WRITELN(CONSTR('-',79));
   GOTOXY(1,22);WRITELN(CONSTR('-',79));
   WRITELN;
   WRITE(CONSTR('-',79));
   INITINDEX;
   GETLOGDRV;    (* ERMITTLUNG DES AKTUELLEN LAUFWERKS. DAS ERGEBNIS  *)
                (* STEHT ALS INTEGER IN LOGDRVNR, ALS CHAR IN LOGDRV *)

(**************************************)

(***************************************)
REPEAT
  OPENFILE(PARMF,'LIPARM.PRM',256);
  IF NOT OK THEN
  BEGIN
     GOTOXY(1,23);
     WRITELN('PARAMETER-DATEI "LIPARM.PRM" NICHT VORHANDEN !');
     WRITE('BITTE STARTEN SIE DAS INSTALLIERUNGSPROGRAMM "LITINST.COM" !');
     CLREOL;
     READ(KBD,CH);
     CLRSCR;
     EXIT;
  END;

  PARM_OPEN:= TRUE;
  GETREC(PARMF,1,G_BUFFER);      (* ALLGEMEINE PARAMETER IN PR LADEN *)
  MOVE(G_BUFFER,PR,SIZEOF(PR));
  BEEP_AKTIV:= PR.BEEP;
  US_T:= PR.USTAST;

  AKT_FORMAT_NR:= 1;  (* ERSTES DRUCKFORMAT ALS VORGABE *)
  SET_DRUCK_PARM_1;

  CLEARFRAME;

  REPEAT
     SELECT('INHALTSVERZEICHNIS ZEIGEN (Y/N)?',['Y','N',#$0D],CH,'N');
     IF CH = 'Y' THEN
        DIRECTORY('????????.???',16); (* STANDARDDATEIEN UND SUBDIRECTORIES *)
  UNTIL CH = 'N';

  REPEAT
     OUTCASE;
     GET_DATNAMES(1);
     IF (PR.TITPFAD[LENGTH(PR.TITPFAD)] IN ['0'..'9']) AND
        (PR.TITPFAD[LENGTH(PR.TITPFAD) - 1] IN ['0'..'9']) THEN
        OK:= TRUE
     ELSE OK:= FALSE;
  UNTIL OK;
  IF PR.IDXPFAD[2] <> ':' THEN
  BEGIN
     AKTDRVNR:= LOGDRVNR;
     AKTDRV:= LOGDRV;
  END
  ELSE
  BEGIN
     AKTDRV:= PR.IDXPFAD[1];
     AKTDRVNR:= ORD(AKTDRV) - 65;
  END;
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
  Y:= 4;      (* FUER AUSGABE AUF ZEILE 4 VORBESETZEN *)
  OPEN_DATS;
  IF NOT ALL_OPEN THEN  (* WENN SICH NICHT ALLE DATEIEN OEFFNEN LIESSEN *)
  BEGIN
     CLOSE_DATS;        (* ALLE BEREITS GEOEFFNETEN DATEIEN SCHLIESSEN *)
     SELECT('PROGRAMM BEENDEN (Y/N)?',['Y','N',#$0D],CH,'N');
     IF CH = 'Y' THEN EXIT;
  END;
UNTIL ALL_OPEN;
(**************************************************************)
IF NOT PR.IBM_DRUCKER THEN
DRUCK_NONIBM(DRUCKPOINTER);

GETFREEGROUPS;
Z25;
CLEARFRAME;
GOTOXY(31,2);
WRITE(' [',CHR(AKTDRVNR + 65),': / 0',DISKNUMMER,' ',TITELDRV,':]');
GOTOXY(50,2);WRITE(INT_TO_STR5(TITEL_ANZ),' TITEL   ',
                   INT_TO_STR5(KEY_ANZ),' SCHLUESSEL');

REPEAT
   SELECT('SELECT: U)PDATE, S)UCHE, L)IST, Q)UIT ?',['U','S','L','Q'],CH,' ');
   CASE CH OF
      'U' : UPDATE;
      'S' : SUCHE;
      'L' : LIST;
   END;
   IF CH <> 'Q' THEN CLEARFRAME;
UNTIL UPCASE(CH) = 'Q';
MOVE(PR,G_BUFFER,SIZEOF(PR));
PUTREC(PARMF,1,G_BUFFER);
CLOSE_DATS;
IF NOT PR.IBM_DRUCKER THEN
DRUCK_IBM(DRUCKPOINTER);
CLRSCR;
END.
