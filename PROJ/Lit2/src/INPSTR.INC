(***************************************************)
TYPE
   INP_STR80    = STRING[80];
   INP_ANYSTR   = STRING[255];
   INP_CHARSET  = SET OF CHAR;
VAR
   BEEP_AKTIV     : BOOLEAN;
   US_T           : BOOLEAN;
   INP_UEBERTRAG  : STR80;

PROCEDURE BEEP;
BEGIN
   IF BEEP_AKTIV THEN
   BEGIN
      SOUND(880);
      DELAY(50);
      NOSOUND;
   END;
END;
(****************************************************)
FUNCTION CONSTR(C: CHAR; N: INTEGER) : INP_STR80;
VAR
   S : STRING[80];
BEGIN
  IF N < 0 THEN
  N:=0;
  S[0]:=CHR(N);
  FILLCHAR(S[1],N,C);
  CONSTR:= S;
END;
(*****************************************************)
PROCEDURE SELECT(    PROMPT   :   INP_STR80;
                     TERM     :   INP_CHARSET;
                 VAR TC       :   CHAR;
                     PRESET   :   CHAR);
VAR
   CH,TA    : CHAR;
   FUNCKEY  : BOOLEAN;
BEGIN
GOTOXY(1,23); WRITE(PROMPT,' ',PRESET);CLREOL;
GOTOXY(LENGTH(PROMPT) + 2,23);
REPEAT
   FUNCKEY:= FALSE;
   READ(KBD,CH);
   TA:= UPCASE(CH);
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
       #79  :  CH:=#$02;      (* END                 = CTRL-B *)
       #73  :  CH:=#$12;      (* PAGE UP             = CTRL-R *)
       #132 :  CH:=#$14;      (* CTRL-PAGE UP        = CTRL-T *)
       #81  :  CH:=#$03;      (* PAGE DOWN           = CTRL-C *)
       #118 :  CH:=#$16;      (* CTRL-PAGE DOWN      = CTRL-V *)
       #59  :  CH:=#$0C;      (* F1                  = CTRL-L *)
       ELSE CH:=#00;
     END;
   TC:= CH;
   IF NOT (TC IN TERM) THEN
      BEEP;
   UNTIL TC IN TERM;
   IF TC = #$0D THEN
   BEGIN
     TC:= PRESET;CH:= PRESET;
   END;
   WRITE(CH);
END;
(*******************************************************)
PROCEDURE INPUTSTR(VAR      S          : INP_ANYSTR;
                            L,X,Y      : INTEGER;
                            TERM       : INP_CHARSET;
                   VAR      TC         : CHAR;
                            K          : INTEGER;
                   VAR      P          : INTEGER;
                            UNDERSCORE : CHAR;
                            US_T       : BOOLEAN);
VAR
   CH,TA      : CHAR;
   FUNCKEY    : BOOLEAN;
   ESZET      : BOOLEAN;
   BK_CNT     : INTEGER;
BEGIN
   INP_UEBERTRAG:= '';

   IF P = -1 THEN
      P:= 0;


   GOTOXY(X + 1,Y + 1);WRITE(S,CONSTR(UNDERSCORE,L - LENGTH(S)));
   ESZET:= FALSE;
   REPEAT
     GOTOXY(X+P+1,Y+1);
     FUNCKEY:=FALSE;
     IF ESZET THEN
     TA:= 'S'
     ELSE READ(KBD,TA);

     IF US_T AND (K <> 7) THEN   (* UMFORMUNG IN DEUTSCHE UMLAUTE *)
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
       #79  :  CH:=#$02;      (* END                 = CTRL-B *)
       #73  :  CH:=#$12;      (* PAGE UP             = CTRL-R *)
       #132 :  CH:=#$14;      (* CTRL-PAGE UP        = CTRL-T *)
       #81  :  CH:=#$03;      (* PAGE DOWN           = CTRL-C *)
       #118 :  CH:=#$16;      (* CTRL-PAGE DOWN      = CTRL-V *)
       #59  :  CH:=#$0C;      (* F1                  = CTRL-L *)
       ELSE CH:=#00;
     END;


     IF (K = 1) OR (K = 3) OR (K = 7) THEN
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
        IF NOT(CH IN [#$01,#$02,#$04,#$05,#$06,#$07,#$0C,#$0D,#$13,#$18,#$19,
        #$08,#$1A,' ','0'..'9']) THEN
           CH:= #00;

     IF K = 4 THEN   (* NUR HEX-ZAHLEN, $ UND BLANK ZUGELASSEN *)
        IF NOT (CH IN [#$04,#$08,#$0D,#13,#$24,' ','0'..'9','A'..'F']) THEN
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
                        ELSE
                           IF K = 6 THEN
                           BEGIN
                             S:= S + CH;
                             WHILE S[P] <> #$20 DO
                                P:= PRED(P);
                             CH:= #$8D;
                           END
                           ELSE
                              BEEP;
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
       #$08,#$7F  :   IF P > 0 THEN              (* CTRL-H = DEL LEFT CHAR. *)
                      BEGIN
                        DELETE(S,P,1);
                        WRITE(#$08,COPY(S,P,L),UNDERSCORE);
                        P:= P - 1;
                      END
                      ELSE
                      BEGIN
                         IF (P = 0) AND (K = 6) THEN
                           P:= -1;
                         BEEP;
                      END;
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

