#!/usr/local/bin/perl

# require 'x86asm.pl';
# &asm_init("cpp","des-586.pl");
# XXX
# XXX
# main'asm_finish

sub main'asm_finish
	{
	&file_end();
	&asm_finish_cpp() if $cpp;
	print &asm_get_output();
	}

sub main'asm_init
	{
	($type,$fn,$i386)=@_;
	$filename=$fn;

	$elf=$cpp=$sol=$aout=$win32=$gaswin=0;
	if (	($type eq "elf"))
		{ $elf=1; require "x86unix.pl"; }
	elsif (	($type eq "a.out"))
		{ $aout=1; require "x86unix.pl"; }
	elsif (	($type eq "gaswin"))
		{ $gaswin=1; $aout=1; require "x86unix.pl"; }
	elsif (	($type eq "sol"))
		{ $sol=1; require "x86unix.pl"; }
	elsif (	($type eq "cpp"))
		{ $cpp=1; require "x86unix.pl"; }
	elsif (	($type eq "win32"))
		{ $win32=1; require "x86ms.pl"; }
	elsif (	($type eq "win32n"))
		{ $win32=1; require "x86nasm.pl"; }
	else
		{
		print STDERR <<"EOF";
Pick one target type from
	elf	- linux, FreeBSD etc
	a.out	- old linux
	sol	- x86 solaris
	cpp	- format so x86unix.cpp can be used
	win32	- Windows 95/Windows NT
	win32n	- Windows 95/Windows NT NASM format
EOF
		exit(1);
		}

	$pic=0;
	for (@ARGV) {	$pic=1 if (/\-[fK]PIC/i);	}

	&asm_init_output();

&comment("Don't even think of reading this code");
&comment("It was automatically generated by $filename");
&comment("Which is a perl program used to generate the x86 assember for");
&comment("any of elf, a.out, BSDI, Win32, gaswin (for GNU as on Win32) or Solaris");
&comment("eric <eay\@cryptsoft.com>");
&comment("");

	$filename =~ s/\.pl$//;
	&file($filename);
	}

sub asm_finish_cpp
	{
	return unless $cpp;

	local($tmp,$i);
	foreach $i (&get_labels())
		{
		$tmp.="#define $i _$i\n";
		}
	print <<"EOF";
/* Run the C pre-processor over this file with one of the following defined
 * ELF - elf object files,
 * OUT - a.out object files,
 * BSDI - BSDI style a.out object files
 * SOL - Solaris style elf
 */

#define TYPE(a,b)       .type   a,b
#define SIZE(a,b)       .size   a,b

#if defined(OUT) || (defined(BSDI) && !defined(ELF))
$tmp
#endif

#ifdef OUT
#define OK	1
#define ALIGN	4
#if defined(__CYGWIN__) || defined(__DJGPP__) || defined(__MINGW32__)
#undef SIZE
#undef TYPE
#define SIZE(a,b)
#define TYPE(a,b)	.def a; .scl 2; .type 32; .endef
#endif /* __CYGWIN || __DJGPP */
#endif

#if defined(BSDI) && !defined(ELF)
#define OK              1
#define ALIGN           4
#undef SIZE
#undef TYPE
#define SIZE(a,b)
#define TYPE(a,b)
#endif

#if defined(ELF) || defined(SOL)
#define OK              1
#define ALIGN           16
#endif

#ifndef OK
You need to define one of
ELF - elf systems - linux-elf, NetBSD and DG-UX
OUT - a.out systems - linux-a.out and FreeBSD
SOL - solaris systems, which are elf with strange comment lines
BSDI - a.out with a very primative version of as.
#endif

/* Let the Assembler begin :-) */
EOF
	}

sub main'align() {} # swallow align statements in 0.9.7 context

1;
