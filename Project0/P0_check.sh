#!/bin/bash
#
# sanity check script for Project 0
#	tarball name
#	tarball contents
#	student identification 
#	makefile targets
#	use of expected functions
#	error free build
#	unrecognized parameters
#	copy stdin to stdout
#	proper --input=
#	proper --output=
#	--segfault
#	--catch --segfault
#
LAB="lab0"
README="README"
MAKEFILE="Makefile"

SOURCES="lab0.c"
SNAPS="backtrace.png breakpoint.png"

EXPECTED="$SOURCES $SNAPS"
SUFFIXES=""

PGM="lab0"
PGMS=$PGM

TIMEOUT=5

# expected return codes
EXIT_OK=0
EXIT_ARG=1
EXIT_BADIN=2
EXIT_BADOUT=3
EXIT_FAULT=4
SIG_SEGFAULT=139

let errors=0

if [ -z "$1" ]
then
	echo usage: $0 your-student-id
	exit 1
else
	student=$1
fi

# make sure the tarball has the right name
tarball="$LAB-$student.tar.gz"
if [ ! -s $tarball ]
then
	echo "ERROR: Unable to find submission tarball:" $tarball
	exit 1
fi

# make sure we can untar it
TEMP="/tmp/TestTemp.$$"
echo "... Using temporary testing directory" $TEMP
function cleanup {
	cd
	rm -rf $TEMP
	exit $1
}

mkdir $TEMP
cp $tarball $TEMP
cd $TEMP
echo "... untaring" $tarbsll
tar xvf $tarball
if [ $? -ne 0 ]
then
	echo "ERROR: Error untarring $tarball"
	cleanup 1
fi

# make sure we find all the expected files
echo "... checking for expected files"
for i in $README $MAKEFILE $EXPECTED
do
	if [ ! -s $i ]
	then
		echo "ERROR: unable to find file" $i
		let errors+=1
	else
		echo "        $i ... OK"
	fi
done

function idString {
	result=`grep $1 $README | cut -d: -f2`
	if [ -z "$result" ]
	then
		echo "ERROR - no $1 in $README";
		let errors+=1
	else
		echo "        $1 ... $result"
	fi
}

# make sure the README contains name and e-mail
echo "... checking for submitter info in $README"
idString "NAME:"
idString "EMAIL:"
idString "ID:"

function makeTarget {
	result=`grep $1: $MAKEFILE`
	if [ $? -ne 0 ]
	then
		echo "ERROR: no $1 target in $MAKEFILE"
		let errors+=1
	else
		echo "        $1 ... OK"
	fi
}

echo "... checking for expected make targets"
makeTarget "clean"
makeTarget "dist"
makeTarget "check"

# make sure we find files with all the expected suffixes
if [ -n "$SUFFIXES" ]; then
	echo "... checking for other files of expected types"
fi
for s in $SUFFIXES
do
	names=`echo *.$s`
	if [ "$names" = '*'.$s ]
	then
		echo "ERROR: unable to find any .$s files"
		let errors+=1
	else
		for f in $names
		do
			echo "        $f ... OK"
		done
	fi
done

# make sure we can build the expected program
echo "... building default target(s)"
make 2> STDERR
RET=$?
if [ $RET -ne 0 ]
then
	echo "ERROR: default make fails RC=$RET"
	let errors+=1
else
	echo "        RC=0 ... OK"
fi
if [ -s STDERR ]
then
	echo "ERROR: make produced output to stderr"
	let errors+=1
else
	echo "        no output to stderr ... OK"
fi

echo "... checking for expected products"
for p in $PGMS
do
	if [ ! -x $p ]
	then
		echo "ERROR: unable to find expected executable" $p
		let errors+=1
	else
		echo "        $p ... OK"
	fi
done

# see if a program returns the expected return code
function testrc {
	if [ $1 -ne $2 ]
	then
		echo "ERROR: expected RC=$2, GOT $1"
		let errors+=1
	else
		echo "        RC=$2 ... OK"
	fi
}

# see if they detect and report invalid arguments
for p in $PGMS
do
	echo "... $p detects/reports bogus arguments"
	./$p --bogus > /dev/null 2>STDERR
	testrc $? $EXIT_ARG
	if [ ! -s STDERR ]
	then
		echo "No Usage message to stderr for --bogus"
		let errors+=1
	else
		echo -n "        "
		cat STDERR
	fi
done

echo "... exercise bad --input from a nonexistent file"
./$PGM --input=NON_EXISTENT_FILE 2>STDERR
testrc $? $EXIT_BADIN
if [ ! -s STDERR ]
then
	echo "No error message to STDERR"
	let errors+=1
else
	echo -n "        "
	cat STDERR
fi

echo "... exercise bad --output to an unwritable file"
touch CANT_TOUCH_THIS
chmod 444 CANT_TOUCH_THIS
./$PGM --output=CANT_TOUCH_THIS 2>STDERR
testrc $? $EXIT_BADOUT
if [ ! -s STDERR ]
then
	echo "No error message to STDERR"
	let errors+=1
else
	echo -n "        "
	cat STDERR
fi
rm -f CANT_TOUCH_THIS

# see if it causes and catches segfaults correctly
echo "... exercise --segfault option"
./$PGM --segfault
testrc $? $SIG_SEGFAULT

echo "... exercise --catch --segfault option"
./$PGM --catch --segfault 2>STDERR
testrc $? $EXIT_FAULT
if [ ! -s STDERR ]
then
	echo "No error message to STDERR"
	let errors+=1
else
	echo -n "        "
	cat STDERR
fi

# generate some pattern data
dd if=/dev/urandom of=RANDOM bs=1024 count=1 2> /dev/null

# exercise normal copy operations
echo "... copy stdin -> stdout"
timeout $TIMEOUT ./$PGM < RANDOM > STDOUT
testrc $? $EXIT_OK
cmp RANDOM STDOUT > /dev/null
if [ $? -eq 0 ]
then
	echo "        data comparison ... OK"
else
	echo "        data comparison ... FAILURE"
	let errors+=1
fi
rm STDOUT

echo "... copy --input -> stdout"
timeout $TIMEOUT ./$PGM --input=RANDOM > STDOUT
testrc $? $EXIT_OK
cmp RANDOM STDOUT > /dev/null
if [ $? -eq 0 ]
then
	echo "        data comparison ... OK"
else
	echo "        data comparison ... FAILURE"
	let errors+=1
fi
rm STDOUT

echo "... copy stdin -> --output"
timeout $TIMEOUT ./$PGM < RANDOM --output=OUTPUT
testrc $? $EXIT_OK
cmp RANDOM OUTPUT > /dev/null
if [ $? -eq 0 ]
then
	echo "        data comparison ... OK"
else
	echo "        data comparison ... FAILURE"
	let errors+=1
fi
rm OUTPUT

echo "... copy --input -> --output"
timeout $TIMEOUT ./$PGM --input=RANDOM --output=OUTPUT
testrc $? $EXIT_OK
cmp RANDOM OUTPUT > /dev/null
if [ $? -eq 0 ]
then
	echo "        data comparison ... OK"
else
	echo "        data comparison ... FAILURE"
	let errors+=1
fi

echo "... use of expected routines"
for r in getopt_long signal strerror close 'dup2\|dup'
do
	c=`grep -c "$r(" *.c`
	if [ $c -gt 0 ]
	then
		echo "        $r ... OK"
	else
		echo "        $r ... NO REFERENCES FOUND"
		let errors+=1
	fi
done
	
# that's all the tests I could think of
echo
if [ $errors -eq 0 ]; then
	echo "SUBMISSION $tarball ... passes sanity check"
	echo
	echo
	cleanup 0
else
	echo "SUBMISSION $tarball ... fails sanity check with $errors errors!"
	echo
	echo
	cleanup -1
fi
