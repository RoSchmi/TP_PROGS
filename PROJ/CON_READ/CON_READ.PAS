(* While learning: Shows a way how to read from the keyboard and write to the screen *)

program CON_READ;
(*$A+,R-,V-*)

uses crt;
var
  readRes : Char;
begin
writeln(' Hello, type a number on the keyboard');

readRes := #0;

while (not ((readRes > #48) and (readRes < #57))) do
begin
  readRes := ReadKey;
  writeln(' Not a number ');
end;
writeln('Congrats, you typed a number. End ');
readRes := ReadKey;
end.
