#   Name: win1250.chs - Windows Code Page 1250
#
#   Description:
#
#       Character classifications for Windows Code Page 1250
#       Character Set Classification Codes:
#        p - printable
#        c - control
#        o - operator
#        x - hex digit
#        d - digit
#        u - upper-case alpha
#        l - lower-case alpha
#        a - non-cased alpha
#        w - whitespace
#        s - name start
#        n - name
#        1 - first byte of double-byte character
#        2 - second byte of double-byte character
#
#   History:
#       15-may-2002 (peeje01)
#           Add 'a' property to DF (referred to as 'Beta')
#           Changed comment to the Unicode Character Name
#           Corrected u/l properties for accented characters starting at D4
#           Added Character Set Classification Code table comments
#	21-May-2002 (hanje04)
#	    Change 'a' property to 's' for DF to correct peeje01 change.
#       13-June-2002 (gupsh01)
#           Added 'l' property for DF, to retain the alphabet property.
#       14-June-2002 (gupsh01)
#           Change 'l' property to 'a' for DF to correct previous change.
#
0-8	c			# Polish Windows 1250 code page
9-D	cw				#HT, LF, VT, FF,CR
20	pw				#space
21-22	po			#!"
23-24	pon			##$
25-2F	po			#%&'()*+,-./
30-39	pdxn		#0123456789
3A-3F	po			#:;<=>?
40	pon				#@
41-46	pxus	61	#ABCDEF
47-5A	pus	67		#GHIJKLMNOPQRSTUVWXYZ
5B-5E	po			#[\]^
5F	ps				#_
60	po				#'
61-66	pxls		#abcdef
67-7A	pls			#ghijklmnopqrstuvwxyz
7B-7F	po			#{|}~
80-8A	po			#
8B	pus	B9			#A z ogonkiem
8C	pus	9C			#S z kreska
8D-9B	po			#
9C	pls				#s z kreska
9D-9E	po			#
9F	pls				#z z kreska
A0-A2	po			#
A3	pus	B3			#L skreslone
A4-AE	po			#
AF	pus	BF			#Z z kropka
B0-B2	po			#
B3	pls				#l skreslone
B4-B8	po			#
B9	pls				#a z ogonkiem
BA-BE	po			#
BF	pls				#z z kropka
C0-C5	po			#
C6	pus	E6			#C z kreska
C7-C9	po			#
CA	pus	E0			#E z ogonkiem
CB-D0	po			#
D1	pus	F1			#N z kreska
D2	po				#
D3	pnu	F3			#O z kreska
D4-D6	pou	F4		# LATIN CAPITAL LETTERs with accents
D7	po			# MULTIPLICATION SIGN
D8-DE	pou	F8		# LATIN CAPITAL LETTERs with accents
DF      psa                     # LATIN SMALL LETTER SHARP S (German)
E0	pls				#e z ogonkiem
E1-E2	po			#
E3	pus	9F			#Z z kreska
E4-E5	po			#
E6	pls				#c z kreska
E7-F0	po			#
F1	pls				#n z kreska
F2	po				#
F3	pnl				#o z kreska
F4-F6	pol			# LATIN SMALL LETTERs with accents
F7	po			# DIVISION SIGN
F8-FE	pol			# LATIN SMALL LETTERs with accents
FF	p			# DOT ABOVE

