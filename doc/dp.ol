Outline for the Data Protocol Document
--------------------------------------

1) Data Passing Overview
2) Data Representation - Structures
	a) Method description
	b) 


-------------------------------------------------------


Data Passing Protocol
---------------------

Third party send/recieve programs written for GeoShare will use the
C structures defined in the geoscience.h header file.  This allows a
simple and familier interface for programmers writing these
programs.  By utilizing the structures directly in send/receive
applications, programmers are able to dictate flexibly the mapping
of their data onto the geoscience model.

Datastructure Programmatic Interface
------------------------------------




Data Structure Dependancy
-------------------------

Since programs will be written to reference the strucures directly,
they will inherently be dependant on particular versions of the
structure definitions they use. 

This problem is primarily handled in the same fashion that
system structure definition dependancy is handled.  Consider the
system header file stdio.h.  Additions to the standard FILE data
structure are extremely rare, but when they are required they are
added to the bottom of the structure such that older programs can
still be compatible by referencing just a subset of the structure.

Changes made to widely published datastructures like the FILE structure
are made only in a carefully arbitrated fashion mediated by a central 
standards commitee that is responsible for re-publishing the definition 
to users.

The GeoShare interface structures must be maintained in this 
fashion so that programs written for the system do not become
obsolete or cease to function when additions are made to the
structure defintions they use.  

There are several fairly obvious rules to observe that will greatly 
simplify the task of maintaining compatibility between various data
structure definition versions:

    *  New structure members must be added only to the bottom 
       of structure definitions.

    *  Existing members must never be deleted or have their data
       type changed.  If a member type becomes obsolete, a new version
       of it must be added to the bottom of the structure.

    *  Definitions of structure members must remain statically
       positioned relative to the start of the structure.
       Reorganization of, or insertion into existing  definitions
       is not allowed.

    *  Structure type names must never be changed. 

If the above rules are observed when modifying the geoscience header
definitions, existing geoshare programs can be utilized without
having to be modified or re-linked.

Programs written for different versions of the interface structures
can be used together with the following considerations:

	* Receivers written against a given version of the interface
	structures may interact only with senders that use the same
	or newer structure versions.

	A receiver that is written against new structure definitions 
	cannot interact with a sender that uses older structure
	definitions unless the receiver does not reference the data
	objects unique to the new version of the structure.


	* Programs may not interact with 

	* A program that uses an older version of 








The sending application would interact with the datastructures in
the following manner:

	* Allocate a structure in shared memory using the gs_alloc
	routine as follows:
		ptr = (struct TheStruct *) gs_alloc(sizof(struct TheStruct),
						&key);
	
	* Fill the structure:
		ptr->Type = type
		ptr->







Alternatively, it is possible to supply programmers with an
interface to the structures that hides their definition.  This
method involves supplying routines that allocate structures,
load structures in a tokenized fashion, and read structures in a
tokenized fashion.  

for instance:
To create a structure the user would make a call like:
	ptr = (Executive_t *) AllocStruct( "Executive_t" )
where the type of structure to create is passed as a string.

To load members of the structure the user would:
	status = LoadStruct( ptr, (MemberId)105, data)

To read from the structure the user would:
	status = ReadStruct(ptr, (MemberId)105, &data);

The MemberId argument identifies the member of the structure to
process.  This id is an integer code read from the Structure
Description Table (SDT).  

The problems associated with this method of handling the
programmatic interface to the structures are:

	* It complicates the design of sender/receiver programs
	by not allowing the programs to marshal their data:

	- Receivers cannot predict the order in which they must
	process data.  Therefor it is difficult to write database
	loading routines that use imbedded SQL calls since building
	the phrases must be done dynamically.

	- Sending routines must fetch data in the order requested by
	the structure interface routines.  This leads to very
	inefficient data handling since programs cannot optimize
	their fetching of data. (i.e. from different databases or
	or other various sources.)  

	





