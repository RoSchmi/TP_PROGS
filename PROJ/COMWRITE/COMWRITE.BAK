(* While learning: Shows how to write to COM1 and write to the screen *)

program COMWRITE;
(*$A+,R-,V-*)

(* A+ = Generate absolute instructions *)
(* R- = BereichsÅberprÅfung aus *)
(* V- = Keine LÑngenÅberprÅfung bei Strings *)
(* I+ = Automatische PrÅfung von Ein-/Ausgaben *)

uses crt, dos;
var
  readRes : Char;
  Lst : Text;
  cmdRes : Integer;
begin
Assign(Lst,'COM1');
{$I-}
Rewrite(Lst);
cmdRes := IOResult;
{$I+}
if (cmdRes <> 0) then
  writeln('Error: Couldnt reset COM1')
  else
  writeln('Resetting COM1 was successful');
writeln('Press key to send to COM1');
readRes := ReadKey;
{$I-}
writeln(Lst, 'A message\r\nA second message\r\nA third message');
cmdRes := IOResult;
{$I+}
if (cmdRes <> 0) then
  writeln('Error: Writing via COM port failed')
  else
  writeln('Writing to COM port was successful');

readRes := #0;
writeln('Type a number to end the program');
while (not ((readRes > #48) and (readRes < #57))) do
begin
readRes := ReadKey;
writeln(' Not a number ');
end;
writeln('Look on the connected device if you have received ');
readRes := ReadKey;
end.



