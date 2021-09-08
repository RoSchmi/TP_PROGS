program rwcpm;
uses crt, dos;

(* Filetransfer from CP/M-86 to DOS *)
(* CP/M Floppies: IBM-Format 8 Sec/Track, 40 Tracks, singlesided *)
(* Based on a Program from M. Ruether and U. Ruether from 1986 *)
(* Compiled with Turbo Pascal 5.0 *)


const 	OFFSET	= 1;
	BLKPERTRK	= 4;
	SECPERBLK	= 2;
	RECPERBLK	= 8;
	RECPEREINT	= 128;
	MAXDIR		= 64;
	MAXBLK		= 155;

type   	deintrag    	= array [0..31] of byte;
	directory 	= array  [1..MAXDIR]  of deintrag;
	rec		= array [0..127] of byte;
	daten		= array [1..128] of rec;
	filename   	= string [64];
	regpack     	= record
				ax, bx, cx, dx, bp, si, di, ds, es, flags: integer;
                       end;

var     dirbuf	: directory;
	datbuf	: daten;
	wahl	: char;
        dummyChar : char;
	msdrv, physdsk, frdircnt, frblkcnt: integer;
	bufvoll	: boolean;
	freidir	:  set of 1..MAXDIR;
	freiblk	: set of 2..MAXBLK;

procedure logdrive (drv : integer);
begin
  if drv = 0 then
  begin
    ChDir('A:');
    writeln('KBytes free on A: ', DiskFree(0) div 1024);
  end
  else
  begin
    ChDir('B:');
    writeln('KBytes free on B: ', DiskFree(0) div 1024);
  end

end;

procedure log(drv: integer);
var 	dummy: array[1..511] of byte;
	{ register: regpack; }
        register : registers;
        retry    : integer;
begin

  retry := 0;
  repeat
  with register do
    begin
	ax  := $0201;		{1 Sektor>
	cx  := $0001;   	{Bootsektor}
	dx  := $0000 + drv; 	{Kopf und Drive}
	es  := seg(dummy);
	bx  := ofs(dummy)
    end;

   intr ($13, register);

   if (register.ax and $FF00) <> 0 then
   begin
        retry := retry + 1;
        if retry < 4 then
        begin
	  writeln ('Read Error DOS-Drive');
          halt
        end
   end
   until register.ax and $FF00 = 0
end;

procedure menu;
begin
	writeln;
	writeln ('Possible Commands:');
	writeln	(' (l)   Show CP/M-Directory');
	writeln	(' (2)   Log in new CP/M-Diskette');
	writeln	(' (3)   Copy File from CP/M to DOS');
	writeln	(' (4)   Copy File from DOS to CP/M');
	writeln	(' (5)   Delete CP/M-File');
	writeln	(' (6)   Use other Disk-Drive for CP/M-Diskette');
	writeln	(' (7)   Programm End');
	writeln	(' (?)   Show this list');
end;


procedure init;
begin
	bufvoll   := false;
	clrscr;
 	writeln  ('Filetransfer from/to CP/M and MS-/PC-DOS');
	writeln;
	repeat
		write  ('Which Drive holds CP/M-Diskette? (0- 3) ');
		readln (physdsk) until physdsk in [0..3];
	repeat
		write ('Which drive holds DOS-Diskette? (0-3) ');
 		readln (msdrv) until msdrv in [0..3]-[physdsk];
		menu
end;

procedure rw_block  (blk:integer; var buffer; command: integer);

var track, sektor, retry: integer;

{ register : regpack; }
register: registers;
begin
	track  := blk div BLKPERTRK + OFFSET;
	sektor :=  (blk mod BLKPERTRK)  * SECPERBLK + 1;
        retry := 0;
	repeat
		with register do
                begin
		ax  :=  (command shl 8)  + 2;      (* 2 Sektoren *)
		cx  :=  (track shl 8) + sektor;
		dx  := $0000 + physdsk;            (* Kopf und Drive *)
		es  := seg(buffer);
		bx  := ofs( buffer)
end;


intr($13,register);
(* writeln ('Bin nach Interrupt Befehl'); *)
if  (register.ax and $FF00)   <>  0 then
	retry := retry + 1;
	if retry >  3 then
          begin
	     writeln  ('Error Disk access CP/M-Disk');
	     halt
          end;
until register.ax and $FF00 = 0
end;

procedure scanfrei;
var i, j, reccount : integer;
begin
	freidir := []; frdircnt  := 0;
	freiblk := [2..MAXBLK]; frblkcnt  := MAXBLK - 1;
	for i  := 1 to MAXDIR do
	     if dirbuf[i,0] = $E5
        then
        begin
	    freidir := freidir + [i];
            frdircnt  := frdircnt + 1
        end
     else
       begin
	reccount  := dirbuf[i,15];
	j  := 16;
	while reccount > 0 do
        begin
		freiblk := freiblk - [dirbuf[i,j]];
		frblkcnt  := frblkcnt - 1;
		j  := j + 1;
		reccount  := reccount - RECPERBLK
        end
      end
     end;


procedure lese_directory;
begin
	rw_block  (0,dirbuf [1,0],2);
	rw_block  (1,dirbuf [33,0],2);
        writeln ('Block read, Files on Drive are: ');
	(* log (msdrv); *)
        logdrive(msdrv);
	bufvoll  := true;
	scanfrei
end;

procedure zeige_directory;
var i,  j: integer;
begin
	if not bufvoll then lese_directory;
	for i  := 1 to MAXDIR do
	if (dirbuf [i,0]  <> $E5) and (dirbuf [i,12] = 0) then
	begin
	for j  := 1 to 8 do write  (chr(dirbuf [i,j]));
        write (' ');
	for j  := 9 to 11 do write (chr(dirbuf [i,j] and $7F)) ;
	write ('   ')
end;
writeln;
writeln (frblkcnt,'  KByte und ',frdircnt,'  Dir-Eintraege frei')
end;

function stupcase(name:filename): filename;
var i: integer;
ausg: filename;
begin
	ausg := '';
	for i:= 1 to length(name)  do ausg := ausg + upcase(name[i]);
	stupcase := ausg
end;

function fname (eintrag:  integer): filename;
	var i: integer;
name: filename;
begin
	i  := 1;
	name := '';
	repeat
		name := name + chr(dirbuf[eintrag,i]);
		i  := i + 1;
	until   (i = 9) or  (dirbuf[eintrag,i] = $20);
	if dirbuf[eintrag,9] and $7F  <> $20 then
	begin
          name := name + '.';
          i  := 9;
          repeat
		name := name + chr(dirbuf[eintrag,i] and $7F);
                i  := i + 1
          until  (i = 12) or ((dirbuf[eintrag,i] and $7F) = $20);
        end;
        fname := name;
end;

function erlaubt(var f:filename): boolean;
const ZSATZ : set of char = ['A'..'Z', '0'..'9', '!', '#'..'&', '+', '-', '/' ];
var i,j,k: integer;
    valid: boolean;
begin
    if length(f)  > 2
    then if f[2] = ':' then delete(f,1,2);
    valid := false;
    k := length (f);
   if k > 0 then if f[1] <> '.' then
   begin i  := 1;
      while (i <=8) and  (i<=k) and  (f[i]  in ZSATZ) do i  := i + 1;
   if i = k + 1 then valid := true
   else if f[i] = '.' then
   begin
       j   := i + 1;
    while (j  <= k) and  (j <= i+3)  and (f[j] in ZSATZ) do j  := j + 1;
    valid :=    (j = k + 1)
    end
end;
erlaubt := valid
end;

function finde_eintrag  (cfname:filename; extent:integer): integer;
var i: integer;
begin
  i := 1;
  while (i <= MAXDIR) and
  ((dirbuf[i,0]=$E5) or (dirbuf[i,12] <> extent) or (fname(i) <> cfname)) do
  i  := i + 1;
  finde_eintrag := i;
end;

procedure kopiere_cm;  (* Copy from CPM to DOS *)
var cfname,  mfname: filename;
mfile: file;
i, j, count, extent,  reccount: integer;
dummyChar : char;
error: boolean;
begin
    if not bufvoll then lese_directory;
    write ('Please enter CP/M-Filename: ');
    While KeyPressed do
    begin
      dummyChar := ReadKey;
    end;
    readln (cfname);
    readln (cfname);
    writeln(cfname);
    cfname := stupcase(cfname);
    if finde_eintrag(cfname,0) <= MAXDIR then
    begin
       repeat
          write ('Please enter DOS-File-Name: ');
          while KeyPressed  do
          begin
            dummyChar := ReadKey;
          end;
          readln (mfname) ;
          assign (mfile, mfname);
     {$I-} rewrite (mfile); {$I+}
          error := ioresult <> 0;
          if error then
             writeln ('DOS-File could not be written!')
       until not error;
       extent  := 0;
       repeat
          i := finde_eintrag(cfname, extent);
          reccount  := dirbuf[i,15];
          count   := 1;
          j  := 16;
          while reccount > 0 do
          begin
             rw_block(dirbuf[i,j], datbuf[count,0], 2);
             j := j + 1;
             count  := count + RECPERBLK;
             reccount  := reccount - RECPERBLK
          end;
          (* log(msdrv); *)
          logdrive(msdrv);
          blockwrite (mfile, datbuf, dirbuf[i,15]);
          extent  := extent + 1

      until dirbuf[i,15] < RECPEREINT;
   close (mfile)
 end
 else writeln ('File not found in Directory!')
end;

procedure namenseintrag  (fname:filename; extent, eintrag:integer);
var i, j : integer;
   fehler: boolean;
begin
	dirbuf[eintrag, 0]   := 0;
        i := 1;
	while  (i   <= length (fname)) and (fname[i] <> '.') do
        begin
           dirbuf[eintrag,i]  := ord(fname[i]);
	i  := i + 1
	end;
        j := i;
       while j <= 8 do begin
           dirbuf[eintrag,j]  := $20;
           j := j + 1;
       end;
       if fname[i] = '.'  then i  := i + 1;
       while (j  <= 11)  and  (i <= length(fname)) do
       begin
         dirbuf[eintrag,j] := ord(fname[i]);
         i  := i + 1;
         j := j + 1;
       end;
       while j <= 11 do
       begin
          dirbuf[eintrag,j]  := $20;
          j  := j + 1
       end;
       dirbuf[eintrag,12] := extent;
       for i  := 13 to 31 do dirbuf[eintrag,i]  := 0
end;

procedure kopiere_mc;   (* Copy from DOS to CP/M-86 *)
var cfname, mfname: filename;
mfile: file;
dummyChar : Char;
reccount, extent, recstoread, datzeiger, eintzeiger,dirzeiger, i, j : integer;
begin
	if not bufvoll then lese_directory;
	write ('Please enter DOS-File-Name: ');
        while KeyPressed do
        begin
          dummyChar := ReadKey;
        end;
	readln  (mfname);
        readln (mfname);
	assign (mfile, mfname) ;
     {$I-} reset(mfile); {$I+}
	if ioresult = 0 then
        begin
	   repeat
	       write ('Please enter CP/M-File-Name: ');
               readln (cfname);
               cfname := stupcase(cfname)
           until erlaubt(cfname);
           reccount := FileSize(mfile);  (* Size of DOS-File *)
           writeln('Filesize is: ');
           writeln(reccount);
           if ((reccount -1 ) div 8 < frblkcnt) and
           (reccount div RECPEREINT < frdircnt) then
           begin
              i  := 0;

              while finde_eintrag(cfname, i) <= MAXDIR do
              begin
		j := finde_eintrag(cfname, i);
		dirbuf[j,0]  := $E5;
		i  := i + 1
              end;
	   scanfrei;
           extent  := 0;
           dirzeiger := 1;
           j  := 1;
           while reccount >= 0 do
           begin
              while not (dirzeiger in freidir) do dirzeiger := dirzeiger + 1;
              freidir := freidir - [dirzeiger];
              frdircnt := frdircnt - 1;
              namenseintrag (cfname, extent, dirzeiger);
              if reccount > RECPEREINT then recstoread := RECPEREINT
                    else recstoread := reccount;
              dirbuf[dirzeiger,15]  := recstoread;
              blockread (mfile,datbuf,recstoread);
              datzeiger := 1;
              eintzeiger := 16;
              while recstoread > 0 do begin
              while not  (j in freiblk) do j := j + 1;
              rw_block(j,datbuf[datzeiger, 0], 3);
              dirbuf[dirzeiger,eintzeiger]  := j;
              freiblk := freiblk - [j];
              frblkcnt := frblkcnt - 1;
              j  := j + 1;
              datzeiger := datzeiger + RECPERBLK;
              eintzeiger := eintzeiger + 1;
              recstoread := recstoread - RECPERBLK
           end;
        (*   log(msdrv); *)
        logdrive (msdrv);
	extent  := extent + 1;
	reccount := reccount - RECPEREINT;
end;
rw_block (0,dirbuf[1,0], 3);
rw_block (1,dirbuf[33,0], 3);
(* log(msdrv) *)
logdrive(msdrv);
end
else writeln ('Diskette or Directory full!')
end
else writeln ('DOS-File not found!')
end;

procedure loesche;
var cfname: filename;
i, j: integer;
dummyChar : char;
begin
	write ('Enter CP/M-File to delete: ');
        while KeyPressed do
        begin
           dummyChar := ReadKey;
        end;
        readln (cfname);
	readln (cfname);
	if length(cfname)  > 0 then begin
	cfname := stupcase(cfname);
        j  := finde_eintrag(cfname,0);
	if j <= MAXDIR then
        begin
           dirbuf[j, 0]  := $E5;
           i := 1;
           while finde_eintrag(cfname,i)  <= MAXDIR do
        begin
           j := finde_eintrag(cfname, i);
           dirbuf[j,0]  := $E5;
           i  := i + 1
           end;
           rw_block (0, dirbuf[1,0], 3);
	   rw_block (1,dirbuf[33,0],3);
             (* log(msdrv); *)
             logdrive(msdrv);
           scanfrei
         end
	 else writeln ('File not found in Directory!')
      end
end;


begin
init;
writeln;
repeat
   	repeat
           write ('Command (1 - 7, ? for Menu): ');
           wahl := '0';
	   read (wahl);
           if not (wahl in ['1'..'7','?']) then writeln ('????')
        until wahl in ['1'..'7','?'];
        case wahl of
           '1': zeige_directory;
           '2': begin
                  lese_directory;
                  zeige_directory;
                end;
           '3' : kopiere_cm;
           '4' : kopiere_mc;
           '5' : loesche;
           '6' : init;
           '?' : menu
        end;
until wahl = '7'
end.
