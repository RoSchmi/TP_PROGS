(* While learning: Shows a way how to read from the COM1 and write to the screen *)

program COM1READ;
(*$A+,R-,V-*)

(* A+ = Generate absolute instructions *)
(* R- = Bereichsberprfung aus *)
(* V- = Keine L„ngenberprfung be Strings *)

uses crt, dos;
var
  readRes : Char;
  Lst : Text;
  est, ioresult : Integer;
begin
Assign(Lst,'COM1');
{$i-}
{ Reset(Lst); }
{ est := IOResult; }
  {i+}
{ writeln(est);  }
{ if est <> 0 then }

writeln('was not Null');

writeln('Io-result was Null');

readRes := ReadKey;
{ read(LST, readRes); }
                                            (* Close(Lst); *)
                                            (* Rewrite(Lst); *)
writeln('Press key to send to COM1');
readRes := ReadKey;
writeln(Lst, 'Eine Textausgabe');
read(Lst, readRes);
{ while ( not Eof(Lst)) do }
begin
  Read(Lst, readRes);
  write(readRes);
end;
readRes := #0;

(* while (not ((readRes > #48) and (readRes < #57))) do *)
(* begin  *)
(*  readRes := ReadKey; *)
(*  writeln(' Not a number '); *)
(* end; *)
writeln('Look if you have received ');
readRes := ReadKey;
end.



