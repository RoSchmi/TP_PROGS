unit AuxInOut;

interface
uses Dos;

procedure AssignAux(var F: Text; Port, Params: Word);

implementation


(*  {$K-,R-} *)

{$R-}

const
  fmClose  = $D7B0;
  fmInput  = $D7B1;
  fmOutput = $D7B2;
  fmInOut  = $D7B3;

type
  TextBuf = array [0..127] of Char;

TextRec = record
        Handle: Word;
        Mode: Word;
        BufSize: Word;
        Private: Word;
        BufPos: Word;
        BufEnd: Word;
        BufPtr: ^TextBuf;
        (* OpenFunc   : Pointer; *)
        (* InOutFunc  : Pointer; *)
        (* FlushFunc  : Pointer; *)
        OpenProc: Pointer;
        InOutProc: Pointer;
        FlushProc: Pointer;
        CloseProc: Pointer;
        AuxPort, AuxParam: Word; { Zwei neue Felder )
        UserFill: array (1. .12] of Byte; { um 4 Bytes kleiner }
        Name: array [0..79] of Char;
        Buffer : TextBuf;
end;


procedure AuxInit(Port, Params : word);
 inline(
   $58/          { POP  AX  ;  Parameter Params -> AX }
   $5A/          { POP  DX  ;  Portnummer -> DX }
   $B4/$00/      { MOV AH,0 ;  Funktionscode: Initialisierung }
   $CD/$14);     { INT 14H  ;  BIOS Aufruf }

function AuxInchar(Port: word): char;
  inline(
   $5A/          { POP AX   ; }
   $B4/$02/      { MOV AH,2 ; }
   $CD/$14);    { INT 14H  ; }

procedure AuxOutchar(Port: word; Ch: char);
 inline(
   $58/          { POP AX   ; }
   $5A/          { POP DX   ; }
   $B4/$01/       { MOV AH,1 ; }
   $CD/$14);     { INT 14H  ; }

function AuxInReady(Port:  word) : boolean;
 inline(
   $5A/          { POP DX   ;  Portnummer -> DX}
   $B4/$03/      { MOV AH,3 ;  Funktionscode: Status }
   $CD/$14/      { INT 14H  ; }
   $88/$E0/      { MOV AL,AH ; Status -> AL }
   $24/$01);     { AND AL,1 ; Daten bereit}

{$F+}


function AuxInput(var F : TextRec) : integer;
 var
   P : integer;
 begin
   with F do
   begin
     P := 0;
     while AuxInReady(AuxPort) and (P<BufSize) do
     begin
       BufPtr^[P]:=AuxInChar(AuxPort); Inc(P);
     end;
     BufPos := 0;
     BufEnd := P;
    end;
     AuxInput := 0;
end;


function AuxOutput(var F: TextRec): integer;
 var
   P: integer;
 begin
   with F do
   begin
   for P := 0 to BufPos-1 do AuxOutChar(AuxPort,BufPtr^[P]);
    BufPos := 0;
   end;
   AuxOutput := 0;
 end;



function AuxIgnore(var F: TextRec) : integer;
 begin
    AuxIgnore := 0;
 end;


function AuxOpen(var F: TextRec) : integer;
 begin
   with F do
   begin
     (* AuxInit(AuxPort,AuxParam); *)
     AuxInit(AuxPort,AuxParam);
     if Mode = fmInput then
     begin
     (*  InOutFunc := @AuxInput;  *)
     (*  FlushFunc := @AuxIgnore; *)
     InOutProc     := @AuxInput;
     FlushProc     := @AuxIgnore;
     end else
   begin
     Mode      := fmOutput;
     (* InOutFunc := @AuxOutput; *)
     (* FlushFunc := @AuxOutput; *)
     InOutProc    := @AuxOutput;
     FlushProc    := @AuxOutput;
   end;
   (* CloseFunc := @AuxIgnore; *)
   CloseProc    := @AuxIgnore;
  end;
  AuxOpen := 0;
 end;

{$F-}
(*
procedure AssignAux;
begin
   with TextRec(F) do
   begin
   Handle:= $FFFF;
   end;
end;
end.
*)


 procedure AssignAux(var F: Text; Port, Params : Word);
 begin
   with TextRec(F) do
   begin
     Handle   := $FFFF;
     Mode     := fmClosed;
     BufSize  := SizeOf(Buffer);
     BufPtr   := @Buffer;
     OpenProc := @AuxOpen;
     AuxPort  := Port;
     AuxParam := Param;
     Name[0]  := #0;
   end;
  end;
 end.


program TestAux;
  uses AuxInOut;
  var
    Com1 : Text;
  begin
    AssignAux(Com1,0,$83);
    Rewrite(Com1);
    Writeln(Com1,'evice drivers are fun!');
    Close(Com1);
  end;

/*

program Testexit;
  var
    ExitSave: pointer;

  {$F+} procedure MyExit ; {$F-}
  begin
    .
    .
    .
    ExitProc := ExitSave;
    ExitProc := MyExit;
    .
    .
    .
  end.

*)












