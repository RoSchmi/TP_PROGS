(* While learning: Shows a way how to read from the COM1 and write to the screen *)

program COM1READ;
(*$A+,R-,V-*)

(* A+ = Generate absolute instructions *)
(* R- = BereichsÅberprÅfung aus *)
(* V- = Keine LÑngenÅberprÅfung be Strings *)

uses crt, dos;
var
  readRes : Char;
  Lst : Text;
  est, ioresult : Integer;
begin
Assign(Lst,'COM1');
{$i-}
Reset(Lst);
{ Open(Lst); }
est := IOResult;
{$i+}
if est <> 0 then
begin
writeln('Error: Io-result (Reset) was not Null');
write(est);
Writeln('');
end
else
writeln('IO-Result (Reset) was Null (o.k)');
writeln('Type a key to continue and send character via COM port');
{$i-}
readRes := ReadKey;
read(LST, readRes);
est := IOResult;
{$i+}
if est <> 0 then
begin
writeln('Io-result(reading char) was not Null');
write(est);
Writeln('');
end
else
begin
writeln('IO-Result (Reading char) was Null (o.k)');
write(readRes);
writeln('');
end;
writeln('After attempt to read first character, Press key to continue');
Reset(Lst);

readRes := ReadKey;

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

Close(Lst);
est := IOResult;
end.



