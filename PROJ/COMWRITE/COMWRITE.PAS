(* While learning: Shows how to write to COM1 and write to the screen *)

program COMWRITE;
(*$A+,R-,V-*)

(* A+ = Generate absolute instructions *)
(* R- = Bereichsberprfung aus *)
(* V- = Keine L„ngenberprfung bei Strings *)
(* I+ = Automatische Prfung von Ein-/Ausgaben *)

uses crt, dos;
var
  readRes : Char;
  Lst : Text;
  writeRes : Integer;
begin
Assign(Lst,'COM1');
Rewrite(Lst);
writeln('Press key to send to COM1');
readRes := ReadKey;
writeln(Lst, 'Eine Textausgabe');
writeRes := IOResult;
if (IOResult <> 0) then
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