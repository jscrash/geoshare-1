#ifndef _GEOSCIENCE_H_DEFINED

#define _GEOSCIENCE_H_DEFINED
/*-----------------------------------------------------------------------------
| 
|   Module:    geoscience.h
|
|   Purpose:   This file contains symbol definitions and structure
|              specifications to be used in data interchange facilities
|              Current working title of system : GeoShare (generic) 
|              Other names: EDICT (Geoquest Systems)
|                           GeoLink (Finder Graphics)
|                           FinderLink (Finder Graphics)
|                           interface.h (No one claims ownership)
| 
|   Notes:     
|
|   History:   1-Feb-1990       S. F. McAdoo,  GQS
|                  Original code.  
|              2-Jun-1990       J. G. Gillespie, Finder Graphics
|                  Enhance 2-D seismic structures
|                  Add null values for seismic survey, fault trace and well structs
|             25-Oct-1990       John Schultz,  GQS
|                  Add new map related structures
|             25-Oct-1990       Matt Hawthorne / Ed Fuss,  GQS
|                  Added preprocessor logic for release 8.0 ( && )
|             29-Oct-1990       Ed Fuss,  GQS
|                  Added more fields to the MAP structures.
|             21-Nov-1990       Ed Fuss,  GQS
|                  Modified the fields to the MAP structures.
|                  Created a new MAP structure, GSMap_t.
|             14-Jan-1991       Ed Fuss,  GQS
|                  Changed name from interface.h to interfacex.h so that
|                  this file will not conflict with 7x source.
|             25-Feb-1991       S. McAdoo, GQS
|                  Made into generic EDICT include file for public use
|              1-Mar-1991       S. McAdoo, GQS J Gillespie, FGS
|                  Reconfigure executive structures
|                  Remove WELL_STRUCT_1, WELL_STRUCT_2
|                  Major shaking of well structures - more to come...
|                  Standardize naming convention
|                      Mixed case type names appended by _t
|                      Upper Case constants preceded by GL_
|                  Changed file name to geoscience.h - awaiting better name
|             13-Mar-1991       S. McAdoo, GQS
|                  Upper Case constants preceded by GS_ in anticipation
|                      that the tentative executive(Pres, VP, etc.) 
|                      level acceptance of the name, GeoShare, sticks.
|                  Made adjustments to the executive level structures.    
|             15-Mar-1991       J Gillespie, FGS Schlumberger
I                               O Osawa, IDS Schlumberger
|                               S Guthery, ASC Schlumberger
|                               H Ganem, ESF Schlumberger
|                               K Waagb�, GECO Schlumberger
|                  Blue Book I
|             21-Mar-1991      S. McAdoo, GQS
|                  An experiment with seperating seismic surfaces from the seismic
|                  data to fit jgg's surface set concept.
|             29-Mar-1991      J Gillespie, FGS
|                  Assert the GS naming convention throughout the file
|                  Add usage notes
|
|-------------------------------------------------------------------------------
|-------------------------------------------------------------------------------
| 
|         This code is the product of of
|         GeoQuest Systems, Inc. and Finder Graphics Systems, Inc
|         and is protected under U.S. Copyright laws.
|         This code may be reproduced in part or whole by any
|         technique or facsimile adoption as long as this notice is included.
| 
+-----------------------------------------------------------------------------*/


/************************************************************************************/
/*                                                                                  */
/*                    Notes on usage of geoscience.h                                */
/*                                                                                  */
/************************************************************************************/
/*
We have assumed an ANSI C model throughout this structure.

On naming conventions

All symbolic (enum) constants begin with GS_ .  All masks begin with GSM_, and all
vector data types begin with GSV_.  All symbolic constants are upper case.

Data typedefs are named using mixed case beginning with GS and ending with _t, as in
GSSurfaceSetName_t.

GSVectors are arrays of objects.  The name of the vector is either the lower-case name
of a typedef with the leading GS and trailing _t stripped off ans underscores appearing
where a case change occurs (e.g. an array of GSDepths_t becomes GSVector_t depths and
GSSurfaceSet_t becomes GSVector_t surface_set) or the name of a scalar value, in which
case the possible element types and sizes are listed in the comment of the variable

On typedefs

Where a data domain is referred to in more than one structure, it is generally given its
own typedef.  This may obscure the reading of the structure for the novice, but assists
in implementing code which asserts domain restrictions, as with object programming
languages (e.g. C++), and helps in the maintainance of the structure. It also assists in
writing code that can be rigorously checked by the compiler.

On surface sets

A surface set is a collection of surfaces usually associated with some geological or
geophysical 'event', such as a seismic horizon or a geological formation or marker. This
surface set is given a name.  In common usage, this corresponds to a formation name
(geological) or a seismic horizon (geophysical).  Surfaces sets can be represented by
grids, contours, scatter (xyz) sets, well picks, seismic line cross- sections.

It is possible to reach the GSSeismicSurface_t structure through two paths.  When the
top-level structure (in Structure_t) is GSSurfaceSet_t, then this top-level structure
defines which surface is being modelled.  In this case, it is an error to enter a
different SurfaceSetName in the GSSeismicSurface_t structure from the top-level
SurfaceSetName.  When the top-level structure is GSSeismicSurvey_t, then any number of
SurfaceSetNames are allowed in the GSSeismicSurface_t structure.  The two questions
being addressed are "Show all data representing this surface" and "Show all data
associated with this seismic survey".

Similarly, it is possible to reach GSWellMarker_t from either the SurfacSet_t structure
or the GSField_t structure.  As with seismic horizons entered through the GSSurfaceSet_t
structure, it is an error to enter a different SurfaceSetName in the GSWellMarker_t
structure from the top-level SurfaceSetName.  When the top-level structure is GSField_t,
then any number of SurfaceSetNames are allowed in the GSWellMarker_t structure.  The two
questions being addressed are "Show all data representing this surface" and "Show all
data associated with this well".

On nulls

Each sender is responsible for setting a null value to be consistently used for integers
and floating point values.  The sender should always fill unknown values with these
constants when the data are not available.  The receiver is obliged to always compare
the data elements with the receiver's null value to determine if the scalar contains a
valid entry.

On strings

Character strings are represented as a vector with element_format = GSV_CHAR,
element_size = sizeof (CHAR) and number_elements = the number of characters in the
string NOT including any trailing null byte.  The strings are NOT guaranteed to be null
terminated.

On receiver specifications

The receiver is expected to state what data types it can support by using the
GSDataSupported_t structure component of the ReceiverSpecs_t structure.  Additionally,
the receiver specifies which of the detail components of the top-level structure it can
process.  This 'hint' can then be used by the sender in preparing a data set for
transmission.  For instance, there is no need to perform a data selection on 3-D seismic
data if the receiver cannot support 3-D data.  It is still permitted to send these data;
the receiver should ignore any data it cannot process.

On status reporting

The receiver is responsible for providing two levels of status reporting: problem or
informational reporting to a user interface, and journalling of results to a logging
'file'.  The former is used to inform the user of a potential problem or exception
condition that may require additional interaction, and the latter is used to chronicle
the successful processing of information. It may be desirable to inform the user at a
specific terminal/workstation about a transfer status and to store summary information
in a central repository, such as the name of a file that was used to store the
information received.

*/
/************************************************************************************/
/*                                                                                  */
/*                    Generally Used Typedefs                                       */
/*                                                                                  */
/************************************************************************************/

typedef long GSLongInt_t;           /* Long integer (usually 4 bytes) */
typedef float GSReal_t;             /* Single precision floating point */
typedef double GSDP_t;              /* Double precision floating point */
typedef unsigned long int GSUlong_t;    /* Unsigned long integer */
typedef long GSBoolean_t;
typedef char *GSVoidptr_t;
typedef long GSMask_t;              /* Bit mask, used to identify item validity in structures */

typedef struct                      /* structure for a rational number */
    {
    GSLongInt_t num;
    GSUlong_t den;
    } GSRational_t;

/************************************************************************************/
/*                                                                                  */
/*                    The Basic Building Block, the GSVector_t                      */
/*                                                                                  */
/************************************************************************************/

/* Basic to the interchange design is that of a vector structure which is
  specified by the following declarations: */

typedef union 
    {
    enum 
        {
        GSV_UNKNOWN = 0,             /* vector type is unknown */
        /* basic data types */
        GSV_INT = 10,                /* vector type GSLongInt_t */
        GSV_REAL = 11,               /* vector type GSReal_t */
        GSV_DP = 12,                 /* vector type GSDP_t */
        GSV_FRAC8 = 13,              /* vector type 8 bit int fraction */
        GSV_FRAC16 = 14,             /* vector type 16 bit int fraction */
        GSV_UNSIGNED_LONG = 15,      /* vector type GSUlong_t */
        GSV_BOOLEAN = 16,            /* vector type GSBoolean_t */
        GSV_RATIONAL = 17,           /* vector type GSRational_t */
        GSV_CHAR = 18,               /* vector type char */
        GSV_COMPLEX = 19,            /* vector type complex */
        GSV_STRING = 20,             /* vector type GSString_t */
        /* executive structure vectors */
        GSV_RECEIVER_RQMTS = 100,    /* vector type GSReceiverRqmts_t */
        GSV_DATA_SUPPORTED = 101,    /* vector type GSDataSupported_t */
        GSV_GSSTRUCTURE = 102,       /* vector type GSStructure_t */
        GSV_RECEIVER_CONTEXT = 103,  /* vector type GSReceiverContext_t */
        GSV_STATUS_RETURN = 104,     /* vector type GSStatusReturn_t */
        /* basic point structures */
        GSV_XYZ_POINT = 1000,        /* vector type GSXYZPoint_t */
        GSV_SEISMIC_DATA_POINT = 1001,/* vector type GSSeismicDataPoint_t */
        GSV_MAP_LOCATION = 1002,     /* vector type GSMapLocation_t */
        GSV_DEPTHS = 1003,           /* vector type GSDepth_t */
        GSV_PROCESS_HISTORY = 1004,  /* vector type GSProcessHistory_t; */
        /* stratigraphic data structures */
        GSV_STRAT_COLUMN = 2000,     /* vector type GSStratigraphicColumn_t */
        GSV_LITHOSTRAT_CODE_DESC = 2001,/* vector type GSLithostratCodeDesc_t */
        /* zone structures */
        GSV_ZONE_VARIABLE_DEF = 3000,/* vector type GSZoneVariableDefinition_t */
        GSV_ZONE_DEF = 3001,         /* vector type GSZoneDefinition_t */
        GSV_ZONE_VALUE = 3002,       /* vector type GSZoneValue_t */
        /* well structures */
        GSV_WELL_REMARKS = 4001,     /* vector type GSWellRemarks_t */
        GSV_WELL_TUBULARS = 4002,    /* vector type GSWellTubulars_t */
        GSV_WELL_PRODUCTION = 4003,  /* vector type GSWellProduction_t */
        GSV_WELL_MARKER = 4004,      /* vector type GSWellMarker_t */
        GSV_WELL_CORE = 4005,        /* vector type GSWellCore_t */
        GSV_WELL_ATTRIBUTE = 4006,   /* vector type GSWellAttribute_t */
        GSV_WELL_MARKER = 4007,      /* vector type GSWellMarker_t */
        GSV_WELL_HDR = 4008,         /* vector type GSWellHdr_t */
        /* well log structures */
        GSV_LOG_RUN = 5000,          /* vector type GSLogRun_t */
        GSV_LOG_PASS = 5001,         /* vector type GSLogPass_t */
        GSV_LOG_SERVICE = 5002,      /* vector type GSLogService_t */
        GSV_LOG_TRACE = 5003,        /* vector type GSLogTrace_t */
        /* seismic data structures */
        GSV_SEIS_TRACE = 6000,       /* vector type GSSeismicTrace_t */
        GSV_SEISMIC_CLASS = 6001,    /* vector type GSSeismicClass_t */
        GSV_SEISMIC_LINE = 6002,     /* vector type GSSeismicLine_t */
        /* map object structures */
        GSV_MAP_POLYLINE = 7000,     /* vector type GSMapPolyline_t */
        GSV_MAP_POINT = 7001,        /* vector type GSMapPoint_t */
        GSV_MAP_SYMBOL = 7002,       /* vector type GSMapSymbol_t */
        GSV_MAP_ANNOTATION = 7003,   /* vector type GSMapAttribute_t */
        GSV_MAP_ATTRIBUTE = 7004,    /* vector type GSMapAnnotation_t */
        GSV_MAP_OBJECTS = 7005,      /* vector type GSMapObjects_t */
        GSV_ATTRIBUTE_ITEM = 7006,   /* vector type GSAttributeItem_t */
        /* seismic interpretation surface structures */
        GSV_CONTACT = 8000,         /* vector type GSContact_t */
        GSV_SEGMENT = 8001,          /* vector type Segment_t */
        GSV_INTERP_CLASS = 8002,     /* vector type InterpretedClass_t */
        GSV_INTERP_LINE = 8003,      /* vector type InterpretedLine_t */
        /* fault trace structures */
        GSV_FAULT_TRACE = 9000,      /* vector type FaultTrace_t */
        GSV_FAULT_SURFACE = 9001,    /* vector type FaultSurface_t */
        /* surface executive structures */
        GSV_SURFACE_ATTRIBUTE = 10000,/* vector type SurfaceAttribute_t */
        GSV_SUFACE_GEOMETRY = 10001 /* vector type SurfaceGeometry_t */
        } symbolic;
    GSLongInt_t extension;
    } GSVectorType_t;

typedef struct 
    {
    GSVectorType_t element_format;
    GSUlong_t element_size;
    GSUlong_t number_elements;
    GSVoidptr_t vector_pointer;
    } GSVector_t;

/*  ALL strings will be stored as vectors,  the typedef GSString_t is defined as a
    convenience and a vector type differentiator */

typedef GSVector_t GSString_t;

typedef GSString_t GSDate_t;        /* Standard date format is YYYYMMDDHHMNNSS.SS
                                       MM from 01 to 12 (month of year)
                                       DD from 01 to 31 (day of month)
                                       HH from 00 to 23 (hour of day)
                                       MN from 00 to 59 (minute of hour)
                                       SS.SS from 0 to 59.99(seconds in minute)
                                       Any component may be truncated
                                       (e.g.April 1987 is entered as 198704) */
/************************************************************************************/
/*                                                                                  */
/*                    STRUCTURE VERSION STAMP                                       */
/*                                                                                  */
/************************************************************************************/

typedef struct 
    {
    GSLongInt_t major;              /* Major version number */
    GSLongInt_t revision;           /* Revision number of structure */
    GSString_t vendor;              /* Vendor that provided latest revision */
    } GSVersion_t;

/************************************************************************************/
/*                                                                                  */
/*                    Data Structure Typedefs                                       */
/*                                                                                  */
/************************************************************************************/

typedef GSLongInt_t GSNodeID_t;     /* node identifier */
typedef GSString_t GSPlotSymbol_t;  /* Graphics plot symbol name */
typedef GSString_t GSUWI_t;         /* Unique Well Identifier */

typedef GSString_t GSSurfaceSetName_t;  /* Name of a surface set */

/* Geophysical data types */

typedef GSRational_t GSShotPoint_t; /* Seismic shot point */
typedef GSRational_t GSCDP_t;       /* Common depth point */
typedef GSRational_t GSLineNumber_t;    /* Seismic line number */
typedef GSReal_t GSStatic_t;        /* Seismic static correction value */

/*
 *       symbols identifying types of interpreted surfaces
 */

typedef enum 
    {
    GS_SRF_TYPE_UNKNOWN = 0,        /* surface type unknown */
    GS_SRF_TYPE_HOR = 1,            /* surface type horizon */
    GS_SRF_TYPE_FLT = 2,            /* surface type fault */
    GS_SRF_TYPE_MARKER = 3          /* surface type (geologic) marker */
    } GSSurfaceType_t;

/* 
 *      Seismic Line type 
 */

typedef enum 
    {
    GS_LINE_2D = 30,                /* line type 2d */
    GS_LINE_3DI = 31,               /* line type 3d inline */
    GS_LINE_XLN = 32,               /* line type 3d cross line */
    GS_LINE_TSL = 33,               /* line type 3d time slice */
    GS_LINE_RANDOM = 34             /* line type 3d random(reconstruction) cut */
    } GSLineType_t;

/*    Processing History structure           
      ---------------------------- */
typedef struct 
    {
    GSDate_t proc_date;
    GSString_t vendor;
    GSString_t description;
    } GSProcessingHistory_t;

/*
 *       variable for flagging whether a polyline is open or closed (a polygon)
 */

typedef enum 
    {
    GS_OPEN_POLYLINE = 0,
    GS_CLOSED_POLYGON = 1
    } GSPolyLineType_t;
/*
 *       masks defining validity of items in struct point
 */

#define GSM_VALID_CDP          (1<<0)   /* cdp number is valid */
#define GSM_VALID_SP           (1<<1)   /* shot point number is valid */
#define GSM_VALID_X            (1<<2)   /* x location is valid */
#define GSM_VALID_Y            (1<<3)   /* y location is valid */
#define GSM_VALID_Z            (1<<4)   /* z location is valid */
#define GSM_VALID_LONGITUDE    (1<<5)   /* longitude is valid */
#define GSM_VALID_LATITUDE     (1<<6)   /* latitude is valid */
#define GSM_VALID_CONNECT      (1<<7)   /* connectivity vector in xyz scatter data */

/* Used in GSDepth_t structure */
#define GSM_VALID_MD           (1<<8)   /* measured depth is valid */
#define GSM_VALID_SUBDATUM     (1<<9)   /* subdatum depth is valid */
#define GSM_VALID_TVD          (1<<10)  /* true vertical depth is valid */
#define GSM_VALID_TIME         (1<<11)  /* time is valid */
#define GSM_VALID_DX           (1<<12)  /* deviation in x direction is valid */
#define GSM_VALID_DY           (1<<13)  /* deviation in y direction is valid */

/************************************************************************************/
/*                                                                                  */
/*                   MEASUREMENT TYPEDEFS                                           */
/*                                                                                  */
/************************************************************************************/

/* 
 *    Units of measure
 */

typedef struct 
    {
    GSString_t type;                /* DISTANCE, TIME, VOLUME, WEIGHT, TEMPERATURE, etc */
    GSString_t unit;                /* Unit (FEET, SECONDS) or expression (G/CC) */
    } GSUnit_t;

typedef struct 
    {
    GSDP_t value;                   /* value of measurement */
    GSUnit_t uom;                   /* unit of measure, including the measurement system and the actual unit */
    } GSMeasurement_t;

typedef GSMeasurement_t GSTemperature_t;    /* Special case of a measurement where uom.type = "TEMPERATURE" */
typedef GSMeasurement_t GSResistivity_t;    /* Special case of a measurement where uom.type = "RESISTIVITY" */
typedef GSMeasurement_t GSTime_t;   /* Special case of a measurement where uom.type = "TIME" */
typedef GSMeasurement_t GSVelocity_t;   /* Special case of a measurement where uom.type = "VELOCITY" */
typedef GSMeasurement_t GSWeight_t; /* Special case of a measurement where uom.type = "WEIGHT" */
typedef GSMeasurement_t GSDistance_t;   /* Special case of a measurement where uom.type = "DISTANCE"
                                         and implication that measurement is made in horizontal sense */
typedef GSMeasurement_t GSElevation_t;  /* Special case of a measurement where uom.type = "DISTANCE"
                                         and implication that measurement is made in vertical sense */

typedef struct 
    {                               /* structure describing reference elevation */
    GSElevation_t val;              /* Elevation to use for subsea corrections */
    GSString_t ref;                 /* Data type being used as elevation reference (KB, DF, GS, SL, etc) */
    } GSElevationRef_t;

/************************************************************************************/
/*                                                                                  */
/*                   POINT STRUCTURES                                               */
/*                                                                                  */
/************************************************************************************/

/*  Other structures are basic to data interchange, that of a single point. 
    The following is the structure for a single point.  A mask will be provided in
    association with each use of this structure (or vector of this structure)
    indicating which components of the structure are valid.  For example, x and y
    may not be necessary to transfer interpretation.  Further the exact meaning of
    the elements of this structure will be context dependent (i. e., the z element
    may contain a time value for interpretation). */

typedef struct 
    {
    GSMask_t valid;                 /* Validity mask for station vector:
                                      GSM_VALID_X, GSM_VALID_Y, GSM_VALID_Z */
    GSDP_t x;                       /* x coordinate value associated with point */
    GSDP_t y;                       /* y coordinate value associated with point */
    GSDP_t z;                       /* z coordinate value associated with point */
    } GSXYZPoint_t;

typedef struct 
    {
    GSMask_t valid;                 /* Validity mask for map point:
                                      GSM_VALID_LONGITUDE, GSM_VALID_LATITUDE */
    GSDP_t longitude;               /* Longitude of map point, in decimal degrees */
    GSDP_t latitude;                /* Latitude of map point, in decimal degrees */
    GSXYZPoint_t point;             /* cartesian points associated with map data point */
    GSNodeID_t node_id;             /* Node id */
    } GSMapLocation_t;

typedef struct 
    {
    GSMask_t valid;                 /* Validity mask for station vector:
                                        GSM_VALID_CDP, GSM_VALID_SP */
    GSCDP_t cdp;                    /* CDP Number associated with point */
    union 
        {
        GSShotPoint_t shot_point;   /* Shot Point Number associated with point (usually 2D) */
        GSLineNumber_t line_number; /* seismic line number (Used for time slices and random sections) */
        } y;
    GSMapLocation_t point;          /* location associated with cdp/shot point */
    } GSSeismicDataPoint_t;

typedef struct 
    {
    GSDP_t xmin;
    GSDP_t ymin;
    GSDP_t zmin;
    
    GSDP_t xmax;
    GSDP_t ymax;
    GSDP_t zmax;
    } GSExtents_t;

/************************************************************************************/
/*                                                                                  */
/*                   Projection & Spheroid Information Structures                   */
/*                                                                                  */
/************************************************************************************/

/*   Projection Information
      ----------------------
 
We will assume all location information contained within the seismic data
sturcture is given in the same coordinate system.  To properly define the
coordinate system the following two structures are used to specify the
projection which was used.  the structures are based on the USGS projection
package. and the projection codes are those used by USGS. */

/************************************************************************

   This file defines the proposed cartographic parameter structure for
   use with 'interface.h'.

************************************************************************/

#define GS_NO_OF_PROJECTIONS (GSLongInt_t)21
#define GS_NO_OF_SPHEROIDS   (GSLongInt_t)20
#define GS_NO_OF_UNITS       (GSLongInt_t)5

/*************************************************************************
Definition of Transformation Parameter Fields by Projection Type Code

Type 0
    No parameters are required for this reference system.
    
Type 1
    1 = Semimajor axis of ellipsoid; if '0.0', the value for Clarke's 1966
        spheroid in meters will be assumed.
    2 = Eccentricity squared of ellipsoid.  If '0.0', a sphere is indicated.
        If > '1.0', field contains semiminor axis of ellipsoid.
    3 = Longitude of any point within the zone.
    4 = Latitude of any point within the UTM zone.
    
    Parms 3 & 4 will be used to determine UTM zone number when a zone
    number of '0' is specified.
    
Type 2
    1 = Semimajor axis of ellipsoid; if '0.0', the value for Clarke's 1966
        spheroid in meters will be assumed.
    2 = Eccentricity squared of ellipsoid.  If '0.0', a sphere is indicated.
        If > '1.0', field contains semiminor axis of ellipsoid.

Type 3,4
    1 = Semimajor axis of ellipsoid; if '0.0', the value for Clarke's 1966
        spheroid in meters will be assumed.
    2 = Eccentricity squared of ellipsoid.  If '0.0', a sphere is indicated.
        If > '1.0', field contains semiminor axis of ellipsoid.
    3 = Latitude of first standard parallel.
    4 = Latitude of second standard parallel.
    5 = Longitude of central meridian.
    6 = Latitude of projection's origin.
    7 = False easting.
    8 = False northing.
    
Type 5,7
    1 = Semimajor axis of ellipsoid; if '0.0', the value for Clarke's 1966
        spheroid in meters will be assumed.
    2 = Eccentricity squared of ellipsoid.  If '0.0', a sphere is indicated.
        If > '1.0', field contains semiminor axis of ellipsoid.
    5 = Longitude of central meridian.
    6 = Latitude of projection's origin.
    7 = False easting.
    8 = False northing.

Type 6
    1 = Semimajor axis of ellipsoid; if '0.0', the value for Clarke's 1966
        spheroid in meters will be assumed.
    2 = Eccentricity squared of ellipsoid.  If '0.0', a sphere is indicated.
        If > '1.0', field contains semiminor axis of ellipsoid.
    5 = Longitude directed straight down below pole of map.
    6 = Latitude of true scale.
    7 = False easting.
    8 = False northing.

Type 8
    Case A - Single Standard Parallel
    1 = Semimajor axis of ellipsoid; if '0.0', the value for Clarke's 1966
        spheroid in meters will be assumed.
    2 = Eccentricity squared of ellipsoid.  If '0.0', a sphere is indicated.
        If > '1.0', field contains semiminor axis of ellipsoid.
    3 = Latitude of standard parallel.
    5 = Longitude of central meridian.
    6 = Latitude of projection's origin.
    7 = False easting.
    8 = False northing.
    9 = 0.0
    
    Case B - Two Standard Parallels
    1 = Semimajor axis of ellipsoid; if '0.0', the value for Clarke's 1966
        spheroid in meters will be assumed.
    2 = Eccentricity squared of ellipsoid.  If '0.0', a sphere is indicated.
        If > '1.0', field contains semiminor axis of ellipsoid.
    3 = Latitude of first standard parallel.
    4 = Latitude of second standard parallel.
    5 = Longitude of central meridian.
    6 = Latitude of projection's origin.
    7 = False easting.
    8 = False northing.
    9 = Any non-zero number.
    
Type 9
    1 = Semimajor axis of ellipsoid; if '0.0', the value for Clarke's 1966
        spheroid in meters will be assumed.
    2 = Eccentricity squared of ellipsoid.  If '0.0', a sphere is indicated.
        If > '1.0', field contains semiminor axis of ellipsoid.
    3 = Scale factor at central meridian.
    5 = Longitude of central meridian.
    6 = Latitude of origin.
    7 = False easting.
    8 = False northing.

Type 10
    1 = Radius of the sphere of reference.  If this field is '0.0', the value 
        6370997.0 meters will be assumed.  This  the radius of a sphere with
        surface area equal to that of Clarke's 1866 ellipsoid.
    5 = Longitude of center of projection.
    6 = Latitude of center of projection.
    7 = False easting.
    8 = False northing.
    
Type 11,12,13,14
    1 = Radius of the sphere of reference.  If this field is '0.0', the value 
        6370997.0 meters will be assumed.
    5 = Longitude of center of projection.
    6 = Latitude of center of projection.
    7 = False easting.
    8 = False northing.

Type 15
    1 = Radius of the sphere of reference.  If this field is '0.0', the value 
        6370997.0 meters will be assumed.
    3 = Height of perspective point above sphere.
    5 = Longitude of center of projection.
    6 = Latitude of center of projection.
    7 = False easting.
    8 = False northing.

Type 16,18,19
    1 = Radius of the sphere of reference.  If this field is '0.0', the value 
        6370997.0 meters will be assumed.
    5 = Longitude of central meridian.
    7 = False easting.
    8 = False northing.

Type 17
    1 = Radius of the sphere of reference.  If this field is '0.0', the value 
        6370997.0 meters will be assumed.
    5 = Longitude of central meridian.
    6 = Latitude of true scale.
    7 = False easting.
    8 = False northing.

Type 20
    Format A
    1 = Semimajor axis of ellipsoid; if '0.0', the value for Clarke's 1966
        spheroid in meters will be assumed.
    2 = Eccentricity squared of ellipsoid.  If '0.0', a sphere is indicated.
        If > '1.0', field contains semiminor axis of ellipsoid.
    3 = Scale factor at center of projection.
    6 = Latitude of origin of projection.
    7 = False easting.
    8 = False northing.
    9 = Longitude of first point defining central geodetic line of projection.
   10 = Latitude of first point defining central geodetic line of projection.
   11 = Longitude of second point defining central geodetic line of projection.
   12 = Latitude of second point defining central geodetic line of projection.
   13 = 0.0
   
    Format B
    1 = Semimajor axis of ellipsoid; if '0.0', the value for Clarke's 1966
        spheroid in meters will be assumed.
    2 = Eccentricity squared of ellipsoid.  If '0.0', a sphere is indicated.
        If > '1.0', field contains semiminor axis of ellipsoid.
    3 = Scale factor at center of projection.
    4 = Angle of azimuth east of north for central line of projection.
    5 = Longitude of point along central line of projection at which
        angle of azimuth is measured.
    6 = Latitude of origin of projection.
    7 = False easting.
    8 = False northing.
   13 = Any non-zero number.


**************************************************************************
Definitions of Known Projection Type Codes

    0  "Geographic"               11  "Lambert Azim. Equal Area"
    1  "Universal Transverse Mercator"
                                  12  "Azimuthal"
    2  "State Plane"              13  "Gnomonic"
    3  "Albers Conical Equal Area"14  "Orthographic"
    4  "Lambert Conformal Conic"  15  "General Vertical Near-Side Perspective"
    5  "Mercator"                 16  "Sinusoidal"
    6  "Polar Stereographic"      17  "Equi-Rectangular"
    7  "Polyconic"                18  "Miller Cylindrical"
    8  "Equidistant Conic"        19  "Van der Grinten"
    9  "Transverse Mercator"      20  "Oblique Mercator"
   10  "Stereographic"

**************************************************************************
Definitions of Known Projection Units

        1    "Feet"                4    Degrees of Arc
        2    "Meters"              5    Packed DMS
        3    "Seconds of Arc"
        
    DMS - packed degrees-minutes-seconds:
        +/-DDDMMSS.SSSSS
        
    where
        DDD is the degrees portion of the angle.
        MM is the minutes portion of the angle.
        SS.SSSSS is the seconds portion of the angle.
        
    The plus sign is optional.  Leading and trailing zeros are optional,
    but embedded zeros must be supplied.

**************************************************************************
Definitions of Zone Codes

    UTM - Standard zones 1-60.
    State Plane - USGS identification codes.
    Other - See notes with Projection Types.
    
**************************************************************************
Projection Name Field

    Normally, the projection names referenced above with the projection
    codes are included in this field.
    
**************************************************************************
Definitions of Known Spheroid Codes

        0    "Clarke 1866"        10    "Modified Everest"
        1    "Clarke 1880"        11    "Modified Airy"
        2    "Bessel 1841"        12    "Walbeck"
        3    "International 1967" 13    "Southeast Asia"
        4    "International 1909" 14    "Australian National"
        5    "WGS 72"             15    "Krassovsky"
        6    "Everest"            16    "Hough"
        7    "WGS 66"             17    "Mercury 1960"
        8    "GRS 1980"           18    "Modified Mercury 1968"
        9    "Airy"               19    "Sphere of Radius 6370997"

*************************************************************************/

typedef struct 
    {                               /* Projection Structure Type */
    GSDP_t parms[15];               /* Transformation Parameters */
    GSLongInt_t projection_type;    /* USGS(or equiv) Projection Type Code */
    GSLongInt_t projection_unit;    /* UOM for Projection. NOTE: NOT used in other
                                     uom character string structures... */
    GSLongInt_t zone_code;          /* UTM or US State Plane Zone */
    GSLongInt_t spheroid_code;      /* USGS (or equiv) Spheroid Code */
    GSString_t proj_name;           /* Projection Name (description) */
    } GSCartoProjection_t;

/************************************************************************************/
/*                                                                                  */
/*                     Surface Geometry Representations                             */
/*                                                                                  */
/************************************************************************************/
/*    xyz scatter Information
      -----------------------
 */

typedef struct 
    {
    GSSurfaceSetName_t surface_set_name;  /* Surface set name */
    GSString_t attribute;           /* name of the attribute being modelled */
    GSUnit_t z_unit;
    
    GSVector_t x;                   /* Vector of x-coordinate values 
                                     (4 or 8 byte floats) */
    GSVector_t y;                   /* Vector of y-coordinate values 
                                     (4 or 8 byte floats) */
    GSVector_t z;                   /* Vector of z-values (4 or 8 byte floats) */
    
    GSMask_t connect_valid;
    GSVector_t connect;             /* Connectivity vector.  If sequences
                                     of data points are intended to be
                                     connected together, then this vector
                                     will be filled.  Normally, this would
                                     take the form of one 'connect' value
                                     for each x,y,z value and would
                                     consist of sequential integer values.
                                     All points intended to be grouped
                                     together would have the same
                                     'connect' value. '0' would indicate a
                                     scatter point not connected to any
                                     other point.  This vector need not be
                                     filled for no connected data values.
                                     (4 byte integer) */
    } GSXYZData_t;

/*    Grid Information
      ----------------
 */
typedef struct 
    {
    /* Basic grid parameters:
    xmin, ymin, xmax, ymax, row-grid distance, col-grid distance
    Note: range values in order min-to-max.  Positive row increment
    for non-rotated grid assumed to increase in positive y-direction.
    Positive column increment for non- rotated grid assumed to increase in
    positive x-direction.  Varying the sign of the row,col increments is the
    mechanism for systems utilizing some corner of the grid other than the
    lower left corner as the grid origin. */
    
    GSSurfaceSetName_t surface_set_name;    /* Surface set name */
    GSString_t attribute;           /* name of the attribute being modelled */
    GSUnit_t z_unit;
    
    GSDistance_t row_distance;      /* distance between rows */
    GSDistance_t col_distance;      /* distance between columns */
    
    GSReal_t angle;                 /* Grid rotation angle measured from the
                                     horizontal.  Angles are in degrees
                                     and open positively in the counter
                                     clockwise direction. */
    GSLongInt_t nrows;              /* Number of rows in the grid. */
    GSLongInt_t ncols;              /* Number of columns in the grid. */
    GSBoolean_t fl_honored;         /* Faults honored during gridding flag.
                                     FALSE: Fault information not used.
                                     TRUE: Fault information was used. */
    GSVector_t zvals;               /* Grid values: Storage order is row 1
                                     to row 'nrows' of column 1 followed
                                     by the row values of columns 2
                                     through column 'ncols'
                                     element_format = GSV_REAL;
                                     element_size = either sizeof (GSReal_t)
                                     or sizeof (GSDP_t) */
    
    GSMask_t valid;                 /* Validity mask for existence of xvals
                                     and yvals that follow (GSM_VALID_X
                                     or GSM_VALID_Y) */
    /* For the following two vectors,
       element_format = GSV_REAL;
       element_size = either sizeof (GSReal_t) or sizeof (GSDP_t) */
    GSVector_t xvals;               /* x-values corresponding to z-values */
    GSVector_t yvals;               /* y-values corresponding to z-values */
    } GSGrid_t;

/*    Contour Information
      -------------------
 */

typedef struct 
    {
    GSSurfaceSetName_t surface_set_name;  /* Surface set name */
    GSString_t attribute;           /* name of the attribute being modelled */
    GSUnit_t z_unit;
    
    GSLongInt_t inc_type;           /* Type contour incrementing used.
                                     0: User specified contour range 
                                    defined by low contour value, 
                                    high contour value and contour
                                    increment.
                                     n: n-contour values specified
                                    explicitly by value. */
    GSVector_t inc_levels;          /* Vector of user specified contour
                                     levels.  The number of levels does not
                                     necessarily match the actual z values
                                     See 'inc_type' above. (4 or 8 byte float) */
    
    GSVector_t z_vals;              /* Z-values for each contour segment
                                     stored.  The number of Z values matches
                                     the number of segments described
                                     in the endpts vector.  The number of Z values 
                                     does not necessarily match the
                                     number of levels described in inc_levels
                                     (4 or 8 byte float) */
    GSVector_t endpts;              /* Vector of zero-based indices in x,y-coordinate
                                     arrays where each contour segment
                                     terminates.  Each value stored in
                                     'endpts' is associated with the
                                     corresponding contour level stored
                                     in 'z_vals'.  (4 byte integer) */
    GSVector_t x;                   /* Vector of contour x-coordinate
                                     values. (4 or 8 byte floats) */
    GSVector_t y;                   /* Vector of contour y-coordinate
                                     values. (4 or 8 byte floats) */
    } GSContour_t;

/************************************************************************************/
/*                                                                                  */
/*            Seismic Geometry Specification and Location Structures                */
/*                                                                                  */
/************************************************************************************/

/*    2-D Survey Geometry Specification and Location
      ---------------------------------------------- */

typedef struct 
    {
    enum 
        {
        GS_REGULAR_CORRESP = 0,
        GS_SEMI_REGULAR_CORRESP = 1,
        GS_IRREGULAR_CORRESP = 2
        } cdp_sp_reg_flag;          /* Flag indicates whether or not a regular
                                         correspondence exists between cdp and sp */
    GSRational_t cdp_per_sp;        /* Nominal number of cdps per shot point */
    GSDistance_t shotpoint_interval;    /* Distance between shot points */
    GSCDP_t cdp1;                   /* Initial cdp (index) number */
    GSShotPoint_t sp1;              /* Initial shot point number */
    GSCDP_t cdpinc;                 /* CDP numbering increment */
    GSShotPoint_t spinc;            /* Shot point numbering increment */
    GSVector_t seismic_data_point;  /* Vector of station location structures
                                     (Not used for surface set - use Segment_t list) */
    } GSSeismicLocation2D_t;

/*    3-D Survey Geometry Specification and Location
      ----------------------------------------------
 
The following structure contains the information necessary to specify a 3d
survey.  If the survey does not exist in the receiving system all of the
information must be provided.  The survey is assumed to be laid out as a
regular orthogonal grid of traces so that knowing the location of one trace,
the trace and line spacing, and the azimuth of the primary lines one can
compute the location of all of the traces. */

typedef struct 
    {
    /*    the following are survey geometry specifications */
    GSGrid_t grid;
    /* grid defines the geometry representation of the 3D survey,
    including the ground distance and the number of values in X and Y.
    Usually the X and Y vectors of the GSGrid_t are not used. */
    GSLineNumber_t lin1;            /* line # of origin inline line (rational) */
    GSLineNumber_t lininc;          /* line number incr (rational) */
    GSLongInt_t number_inlines;     /* number of inlines in survey */
    GSCDP_t cdp1;                   /* origin cdp on inline lines.(rational) */
    GSCDP_t cdpinc;                 /* cdp incr. on inline lines (rational) */
    GSLongInt_t number_cdps;        /* number of cdps in each inline */
    GSShotPoint_t sp1;              /* sp of survey origin (rational) */
    GSShotPoint_t spinc;            /* sp incr. of survey  (rational) */
    
    /*    the following are coordinates with which to locate the survey */
    /* Enter in a clockwise sense; DO NOT assume grid is orthonormal */
    GSSeismicDataPoint_t loc[4];
    } GSSeismicLocation3D_t;

/************************************************************************************/
/*                                                                                  */
/*            Seismic Survey Structures                                             */
/*                                                                                  */
/************************************************************************************/

/*    Seismic Data Structures
      -----------------------
 
Seismic data will be contained in the interchange structure as a seismic line
header structure and a vector of trace structures.  The trace structure
contains a trace header structure and a vector of sample values. */

/*    Line Header                                 
      ----------- */
typedef struct 
    {
    GSUnit_t si_unit;               /* units for sampling interval */
    
    enum 
        {
        GS_TRACE_ORDER_SHOT = 1,    /* shot ordered */
        GS_TRACE_ORDER_CDP = 2,     /* unstacked cdps */
        GS_TRACE_ORDER_STACK = 3,   /* stacked cdps */
        GS_TRACE_ORDER_SEQ = 4      /* trace sequential */
        } trace_order;              /* Trace ordering */
    
    GSUnit_t to_unit;               /* trace offset units */
    
    GSBoolean_t offset_validity;    /* TRUE = offsets are valid */
    
    GSDistance_t trail_offset;      /* trailing trace offset */
    GSDistance_t lead_offset;       /* leading trace offset */
    GSDistance_t near_offset;       /* ABS near trace offset */
    GSDistance_t far_offset;        /* ABS far trace offset */
    
    GSReal_t sample_interval;       /* sampling rate */
    GSLongInt_t n_samples;          /* maximum number of samples per trace */
    GSLongInt_t n_live_traces;      /* number of live traces */
    GSUlong_t n_live_samps;         /* number of live samples */
    
    GSLongInt_t n_channels;         /* number of channels per record  (1) */
    GSLongInt_t n_fold;             /* stack fold (no. of traces per cdp) */
    
    GSDistance_t shotpoint_interval;    /* distance bewteen shots (offset unit) */
    GSDistance_t receiver_interval; /* distance between receivers */
    GSDistance_t cdp_interval;      /* distance between cdps */
    
    GSSeismicLocation2D_t line_numbering;    /* specification of cdp/shotpoint numbering */
    
    GSShotPoint_t min_shotpoint;    /* minimum shotpoint */
    GSShotPoint_t max_shotpoint;    /* maximum shotpoint */
    GSRational_t min_receiver;      /* minimum receiver location */
    GSRational_t max_receiver;      /* maximum receiver location */
    GSCDP_t min_cdp;                /* minimum cdp */
    GSCDP_t max_cdp;                /* maximum cdp */
    
    GSBoolean_t cdps_valid;         /* TRUE = linear progression of cdps to lcs */
    GSRational_t cdp_incr;          /* cdp number increment per trace */
    GSBoolean_t shotpoint_valid;    /* TRUE = linear progression of sps to lcs */
    GSRational_t shotpoint_incr;    /* shotpoint increment per trace */
    
    GSReal_t max_amplitude;         /* volume statistics */
    GSReal_t min_amplitude;
    GSReal_t max_magnitude;
    GSReal_t min_magnitude;
    GSReal_t rms_amplitude;
    GSReal_t mean_amplitude;
    GSReal_t mean_magnitude;
    enum 
        {
        GS_NO_MIGRATION = 0,
        GS_PRESTACK_MIGRATION = 1,
        GS_POSTSTACK_MIGRATION = 2
        } migration_type;
    } GSSeismicLineHdr_t;

/*    Trace Header                                 
      ------------ */

typedef struct 
    {
    enum 
        {
        GS_DEAD_TRACE = -1,
        GS_LIVE_TRACE = 1,
        GS_SWEEP_TRACE = 2,
        GS_NULL_TRACE = 3
        } trace_type;               /* activity of trace (live, dead, etc) */
    GSCDP_t cdp;                    /* common midpoint number (cmp) */
    GSLongInt_t cdp_trace_no;       /* cdp trace number */
    GSLongInt_t cdp_fold;           /* stacking fold at this cdp */
    GSShotPoint_t cdp_sp;           /* shotpoint corresponding to this cdp */
    
    GSElevation_t cdp_datum;        /* cdp datum */
    GSElevation_t cdp_wdepth;       /* depth of weathering layer at CDP location */
    GSVelocity_t cdp_wvel;          /* velocity of weathering layer at CDP location */
    GSMapLocation_t cdp_xyz;        /* cdp location & elevation */
    GSDistance_t distance;          /* signed trace distance from shot to receiver
                                     with respect to line direction */
    GSLongInt_t mute_length;        /* length of initial mute is samples */
    GSReal_t tfs;                   /* time of first sample in sample units */
    
    GSRational_t shot_location;     /* shotpoint location number */
    GSElevation_t shot_datum;       /* shotpoint elevation datum */
    GSElevation_t shot_wdepth;      /* depth of weathering layer at SP */
    GSVelocity_t shot_wvel;         /* velocity of weathering layer at SP */
    GSMapLocation_t shot_xyz;       /* shotpoint location & elevation */
    GSElevation_t shot_depth;       /* depth of shot */
    GSReal_t uphole_time;           /* shot uphole time in sampling interval unit */
    
    GSRational_t receiver_location; /* receiver location number */
    GSElevation_t receiver_datum;   /* receiver elevation datum */
    GSElevation_t receiver_wdepth;  /* depth of weathering layer at receiver location */
    GSVelocity_t receiver_wvel;     /* velocity of weathering layer at receiver location */
    GSMapLocation_t receiver_xyz;   /* receiver location & elevation */
    
    GSLongInt_t statics_flag;       /* statics application bit flag */
    /* 0 - shot W statics computed */
    /* 1 - shot W statics applied */
    /* 2 - receiver W statics computed */
    /* 3 - receiver W statics applied */
    /* 4 - shot D static computed */
    /* 5 - shot D static applied */
    /* 6 - receiver D static computed */
    /* 7 - receiver D static applied */
    /* 8 - shot A static computed */
    /* 9 - shot A static applied */
    /* 10- receiver A static computed */
    /* 11- receiver A static applied */
    /* 12- cdp A static computed */
    /* 13- cdp A static applied */
    /* 14- bulk static computed */
    /* 15- bulk static applied */
    GSStatic_t receiver_wstatic;    /* receiver weathering static */
    GSStatic_t receiver_dstatic;    /* receiver datum static */
    GSStatic_t receiver_astatic;    /* receiver automatic static */
    GSStatic_t shot_wstatic;        /* shot weathering static */
    GSStatic_t shot_dstatic;        /* shot datum static */
    GSStatic_t shot_astatic;        /* shot automatic static */
    GSStatic_t cdp_astatic;         /* cdp automatic static */
    GSStatic_t bulk_static;         /* bulk static */
    
    GSLongInt_t record_no;          /* sequential record number */
    GSLongInt_t record_trace_no;    /* record trace number */
    GSLongInt_t field_record_no;    /* field record number */
    GSLongInt_t field_record_trace_no;
    
    GSLongInt_t n_live;             /* number of non-zero samples */
    GSReal_t max_amplitude;         /* maximum signed amplitude */
    GSReal_t min_amplitude;         /* minimum signed amplitude */
    GSReal_t max_magnitude;         /* maximum magnitude */
    GSReal_t min_magnitude;         /* minimum magnitude */
    GSReal_t mean_amplitude;        /* mean trace amplitude */
    GSReal_t mean_magnitude;        /* mean magnitude of trace */
    GSReal_t rms_amplitude;         /* rms amplitude of trace */
    
    GSReal_t minimum_scalar;        /* scalar values used in scaling data to */
    GSReal_t maximum_scalar;        /* fraction values */
    
    GSLongInt_t user_1;             /* user trace header values */
    GSLongInt_t user_2;
    GSLongInt_t user_3;
    GSLongInt_t user_4;
    } GSSeismicTraceHdr_t;

/*    Trace                                  
      ----- */

typedef struct 
    {
    GSMapLocation_t trace_loc;      /* map location of trace */
    GSSeismicTraceHdr_t header;     /* Structure containing seismic trace header */
    GSVector_t samples;             /* Vector of seismic trace samples
                                     (see vector descriptor for sample format) -
                                    (Note: no corresponding Samples_t exists) */
    } GSSeismicTrace_t;

/*    Line Description Structure
      --------------------------
 
All surveys are organized as a series of lines and the survey structure
includes a vector of line structures of the following form.  Each line may have
several variants usually depending on any processes which have been applied to
the data to alter its presentation or to extract some quality of interest. 
Therefore the line structure contains location information and a vector of
classes.  There may be many types of data related to a line class.  Each type 
will have a structure ( or vector of structures ) in which that type of data 
is stored.  Included in the current proposal is a vector of interpreted 
surfaces and seismic trace data. */

typedef struct 
    {
    GSLineType_t line_type;         /* Seismic Line type */
    GSString_t line_name;           /* 2d or reconstruction cut line name */
    GSLongInt_t line_number;        /* Line number (required for GS_LINE_3DI,
                                       GS_LINE_TSL, GS_LINE_XLN) */
    GSString_t line_desc;           /* Line description */
    GSVector_t seismic_class;       /* Vector of seismic line variants delineated
                                       according to class */
    GSSeismicLocation2D_t loc;      /* GS_LINE_2D, GS_LINE_RCN types only */
    } GSSeismicLine_t;

/*    GSSeismic Class structure                
      ----------------------- */
/************************************************************************************/

typedef struct 
    {
    GSString_t class_name;          /* User assigned class name */
    GSString_t class_desc;          /* Class description */
    GSSeismicLineHdr_t seismic_line_hdr;    /* Structure containing seismic line header */
    GSVector_t seismic_trace;       /* Vector of seismic traces */
    GSVector_t seismic_surface;     /* Vector of seismic surfaces */
    GSVector_t processing_history;  /* Seismic trace file processing history */
    } GSSeismicClass_t;

/************************************************************************************/
/*                                                                                  */
/*                          SEISMIC SURFACE STRUCTURES                              */
/*                                                                                  */
/************************************************************************************/

/*    
It will be assumed that there are two basic types of surfaces, horizons and
faults (or fault cuts).  Along any line there may be line segments indicating
where the surface has been interpreted and there may be information indicating
where two surfaces contact each other.  Additionally, the segments may have
different data depending on the z value contained therein.  For example, the
user may have interpreted a horizon along a range of cdps and saved the time
values at which the horizon occured.  He may have also saved the seismic trace
amplitude at that time.  Segments containing both time and containing amplitude
may be stored for a given surface. */

/*  This structure contains information specifying points along the surface at
    which the user has indicated that it intersects another surface.  The structure
    includes the location of the intersection and information about the
    intersecting structure. */

typedef struct 
    {
    GSSurfaceType_t surface_type;   /* Surface type: 
                                       GS_SRF_TYPE_HOR, GS_SRF_TYPE_FLT */
    GSSurfaceSetName_t surface_set_name;    /* Surface set name */
    GSUnit_t z_uom;                 /* Unit of measurement for z */
    GSString_t source;              /* Name of interpreter or source of data */
    GSString_t surface_desc;        /* Surface description */
    GSDate_t modified_date;         /* date this surface was last modified */
    
    /* The following are applicable only to horizon surfaces */
    enum 
        {
        GS_SNAP_P_NEAR = 1,         /* nearest peak to center of window */
        GS_SNAP_P_MAX = 2,          /* largest peak in window */
        GS_SNAP_T_NEAR = 3,         /* nearest trough to center of window */
        GS_SNAP_T_MAX = 4,          /* largest trough in window */
        GS_SNAP_ZPN = 5,            /* zero crossing, positive to negative */
        GS_SNAP_ZNP = 6,            /* zero crossing, negative to positive */
        GS_SNAP_NULL = 7,           /* snap to input point */
        GS_SNAP_UNDEFINED = 8       /* undefined snap type */
        } snap_event;
    GSReal_t snap_window;           /* Duration of snapping window in seconds */
    GSVector_t segment;             /* Vector of interpretation segments for line */
    GSVector_t contact;             /* Vector of contact points associated with this
                                     surface on this line */
    } GSSeismicSurface_t;

typedef struct 
    {
    GSSeismicDataPoint_t loc;       /* location of contact point */
    GSSurfaceType_t surface_type_2; /* Surface type: 
                                      GS_SRF_TYPE_HOR, GS_SRF_TYPE_FLT,
                                      GS_SRF_TYPE_UNKNOWN */
    GSString_t name_2;              /* second name associated with contact */
    } GSContact_t;

typedef struct 
    {
    GSLongInt_t seg_id;             /* Number identifying segment */
    enum                            /* Type of z data in point vector */
        {
        GS_Z_TYPE_UNDEFINED = 0,
        GS_TIME_UNSNAPPED = 1,      /* Z time is original picks */
        GS_TIME_REFINED = 2,        /* Z time is snapped and smoothed */
        GS_AMPLITUDE = 3            /* Z is amplitude from class of 
                                       seismic data associated with surface */
        } z_type;
    GSVector_t seismic_data_point;  /* Vector of points in interpretation segment */
    } GSSegment_t;

/************************************************************************************/
/*                                                                                  */
/*                          FAULT TRACE STRUCTURES                                  */
/*                                                                                  */
/************************************************************************************/

/*    
Fault traces are interpreted information defining the map view of the
intersection of faults with a particular surface.  The interpretation is
usually done on a map and cannot be organized on a line by line basis or, for
that matter, on a survey basis.  Fault trace interpretation, therefore, must 
be kept in a structure separate from the survey structure described above and 
transmitted seperately.  The following structures will be used to contain and
transmit fault trace information.  The top structure contains information
necessary to define the coordinate system of the data contained in the
structure and a vector of surface structures for which fault trace information
is transmitted. */

/*  Each surface may have intersections with many faults.  The following 
    structure names a vector of fault traces for a specified surface. */

typedef struct 
    {
    GSSurfaceSetName_t surface_set_name;    /* Name of surface for which traces are
                                               defined */
    GSVector_t fault_trace;         /* Vector of structures defining fault traces */
    } GSFaultSurface_t;

/*  The following fault trace vector specifies the name of the intersecting fault
    and names the vector containing the points associated with the 
    intersection. */

typedef struct 
    {
    GSSurfaceSetName_t fault_name;  /* Name of fault whose intersection with the surface
                                       this trace describes */
    GSLongInt_t ft_seg_id;          /* segment id for fault trace for a given surface */
    enum                            /* z value code */
        {
        GS_Z_VALUE_UNDEFINED = 0,   /* no z value is defined */
        GS_Z_VALUE_TIME = 1,        /* z value is time */
        GS_Z_VALUE_UNSNAPPED_TIME = 2,  /* z value is unsnapped time */
        GS_Z_VALUE_DEPTH = 3        /* z value is depth */
        } z_value_code;
    GSPolyLineType_t open_closed;
    GSVector_t map_location;        /* Vector of structures defining fault trace points */
    } GSFaultTrace_t;

/************************************************************************************/
/*                                                                                  */
/*                          SURFACE  STRUCTURES                                     */
/*                                                                                  */
/************************************************************************************/

typedef struct 
    {
    enum 
        {
        GS_REP_NONE,
        GS_REP_GRID,
        GS_REP_SEISMIC,
        GS_REP_XYZ,
        GS_REP_CONSTANT,
        GS_REP_CONTOUR,
        GS_REP_WELL_MARKER
        } type;
    union 
        {
        GSGrid_t grid;
        GSSeismicSurface_t seismic;
        GSXYZData_t xyz;            /* 'Scatter' data file */
        GSMeasurement_t constant;
        GSContour_t contour;
        GSVector_t well_marker;     /* Used for marker geometry */
        GSVector_t well_attribute;  /* Used for secondary attributes at marker location */
        } data;
    } GSSurfaceRepresentation_t;

typedef struct 
    {
    GSString_t attribute;           /* name of the attribute being modelled */
    GSVector_t surface_representation;  /* list of attribute representations */
    } GSSurfaceAttribute_t;

typedef struct 
    {
    GSExtents_t extent;             /* XYZ spanning extent of data set */
    GSSurfaceRepresentation_t time_representation;  /* set of coordinates describing
                                                     the geometry of the surface in time */
    GSSurfaceRepresentation_t depth_representation; /* set of coordinates describing
                                                     the geometry of the surface in depth */
    GSVector_t surface_attribute;   /* List of attributes for this surface
                                       which correspond to, or are measured at,
                                         the geometry specified in either time or depth */
    } GSSurfaceGeometry_t;
/************************************************************************************/
/*                                                                                  */
/*                               MAP STRUCTURES                                     */
/*                                                                                  */
/************************************************************************************/

/*    Attribute Information For Map Objects
      -------------------------------------

Each map object defined in a mapping database can have associated
non-graphic attribute information.  This is simply a list of name-value
pairs which are carried along with the map object. */

typedef struct 
    {
    GSString_t item_name;           /* name of attribute */
    GSMeasurement_t item_value;     /* value and unit of measure of attribute */
    } GSAttributeItem_t;

/*    Data Source For Map Data
      ------------------------

It is often useful to know where a certain set of map data came from
(e.g. Tobin, PI, etc.).  The map data source is simply a name that can 
be associated with a map object in the database (it is optional). */

/*    Data Category Of A Map Object
      -----------------------------

Each map object added to the database must be associated with a user supplied
category name (e.g. rivers, coast line, etc.).  The user should be careful
to avoid spelling variations (e.g. "RIVER" and "RIVERS" are all
treated as separate categories). */

/* The following structure describes an annotation map object */

typedef struct 
    {
    GSString_t category_name;       /* Name of the data category. A null value is
                                       NOT allowed. */
    enum 
        {
        GS_PRIMARY_ANNOTATION = 0,
        GS_ATTRIBUTE_ANNOTATION = 1
        } anno_type;                /* Primary or attribute annotation ??? */
    GSString_t anno;                /* Text */
    GSString_t font;                /* Annotation font */
    GSReal_t height_in_mm;          /* Nominal display height (in mm) */
    GSReal_t anchor_angle;          /* Angle for annotation display */
    GSMapLocation_t anchor_point;   /* Anchor location */
    GSVector_t attribute_item;      /* Optional list of attributes */
    } GSMapAnnotation_t;

/* The following structure describes a symbol map object */

typedef struct 
    {
    GSString_t category_name;       /* Name of the data category. A null value is
                                       NOT allowed. */
    GSPlotSymbol_t symbol;          /* Plot Symbol */
    GSReal_t height_in_mm;          /* Nominal display height (in mm) */
    GSMapLocation_t anchor_point;   /* Anchor location */
    GSReal_t anchor_angle;          /* Angle for annotation display */
    GSVector_t attribute_item;      /* Optional list of attributes */
    } GSMapSymbol_t;

/* The following structure describes a point map object */

typedef struct 
    {
    GSString_t category_name;       /* Name of the data category. A null value is
                                       NOT allowed. */
    GSMapLocation_t location;       /* Point location */
    GSVector_t attribute_item;      /* Optional list of attributes */
    } GSMapPoint_t;

/* The following structure describes a polyline map object */

typedef struct 
    {
    GSString_t category_name;       /* Name of the data category. A null value is
                                       NOT allowed. */
    GSBoolean_t flag;               /* True = add additional buffers of polyline data. 
                                     False = only one buffer of polyline data. */
    GSPolyLineType_t open_closed;   /* Open polyline or polygon ? */
    GSVector_t map_location;        /* List of locations */
    GSVector_t attribute_item;      /* Optional list of attributes */
    } GSMapPolyline_t;

/* The following structure describes a map object */

typedef struct 
    {
    GSVector_t map_annotation;      /* NOTE: type = GSV_MAP_ANNOTATION */
    GSVector_t map_symbol;          /* NOTE: type = GSV_MAP_SYMBOL */
    GSVector_t map_point;           /* NOTE: type = GSV_MAP_POINT */
    GSVector_t map_polyline;        /* NOTE: type = GSV_MAP_POLYLINE */
    } GSMapObjects_t;

/************************************************************************************/
/*                                                                                  */
/*                          LEGAL DESCRIPTION STRUCTURE                             */
/*                                                                                  */
/************************************************************************************/

typedef struct 
    {
    enum 
        {
        GS_JEFFERSONIAN,
        GS_GULF_COAST_OFFSHORE,
        GS_TEXAS_ABSTRACT,
        GS_OCS_OFFSHORE
        } type;
    union 
        {
        struct 
            {                       /* structure describing Jeffersonian description of
                                       a location */
            GSLongInt_t meridian_code;  /* PRINCIPAL_MERIDIAN code number - (Note: we
                                         need to publish a list or cite a reference - jgg) */
            GSReal_t township;      /* township number */
            char twpns;             /* 'N' or 'S' of survey base line */
            GSReal_t range;         /* range number */
            char rngew;             /* 'E' or 'W' of survey principal meridian */
            GSReal_t section;       /* section number in township */
            GSString_t section_type;    /* type of 'section' (NULL = section, 'LOT', etc */
            GSString_t spot;        /* Point in section from which measurements are references */
            GSString_t state;       /* State or province abbreviation */
            GSString_t country;     /* Country abbreviation */
            GSString_t county;      /* County, district, parrish name */
            GSString_t narrative;   /* Prose description of location */
            } jeffersonian;
        
        struct 
            {
            GSString_t abstract;    /* Abstract ID */
            GSString_t state;       /* State or province abbreviation */
            GSString_t country;     /* Country abbreviation */
            GSString_t county;      /* County name */
            } texas_abstract;
        
        struct 
            {
            GSString_t tract;       /* Tract name */
            struct 
                {
                GSLongInt_t number; /* Numeric part of block identifier */
                char alpha;         /* Optional character part of block identifier */
                } block_id;
            } gulfcoast_offshore;
        struct 
            {
            enum 
                {
                GS_FEDERAL_WATERS,  /* Federal zone (non-state) */
                GS_STATE_WATERS     /* State zone */
                } state_fed;        /* Flag indicating whether point is under state or
                                       federal control */
            GSString_t offshore_block;  /* Offshore block number */
            GSString_t utm_grid_bloc;   /* Universal Transverse Mercator block
                                           identifier */
            GSLongInt_t ocs_number;
            } ocs_offshore;         /* Outer Continental Shelf description */
        
        } system;
    GSDistance_t nsfootage;         /* 'footage' in north-south direction */
    GSString_t ns;                  /* reference line of north-south 'footage'
                                       ('FEL','FWL') */
    GSDistance_t ewfootage;         /* 'footage' in ease-west direction */
    GSString_t ew;                  /* reference line of east-west 'footage'
                                       ('FNL','FSL') */
    } GSLegal_t;

/************************************************************************************/
/*                                                                                  */
/*                             GENERAL WELL STRUCTURES                              */
/*                                                                                  */
/************************************************************************************/

typedef struct 
    {                               /* structure for defining well depths */
    GSMask_t valid;                 /* Indicates which fields contain valid information */
    GSReal_t md;                    /* measured depth */
    GSReal_t subdatum;              /* depth relative to local datum */
    GSReal_t tvd;                   /* true vertical depth */
    GSReal_t time;                  /* two-way travel time */
    GSReal_t dx;                    /* X deviation from top-hole position */
    GSReal_t dy;                    /* Y deviation from top-hole position */
    } GSDepth_t;

typedef struct 
    {                               /* structure for lithostrat codes (mandatory) */
    GSString_t Code;                /* Formation code of the lithostratigraphic unit */
    GSString_t Age;                 /* Client-specified age code of
                                     lithostratigraphic unit (optional) */
    } GSLithoStrat_t;

/************************************************************************************/
/*                                                                                  */
/*                            WELL HEADER STRUCTURE                                 */
/*                                                                                  */
/************************************************************************************/

typedef struct 
    {                               /* structure defining well header information */
    GSVersion_t Version;            /* version number of this data structure */
    GSUWI_t uwi;                    /* Unique Well Identifier (mandatory) */
    GSPlotSymbol_t symbol;          /* Plot Symbol name */
    
    GSUnit_t z_unit;                /* Unit of measure for all depths */
    
    struct 
        {
        GSBoolean_t flag;           /* TRUE if well is deviated */
        GSVector_t depth;           /* vector of GSDepth_t structures */
        } deviation;
    
    GSString_t well_name;           /* Full legal name of well lease */
    GSString_t well_number;         /* Well permit number within lease */
    GSString_t plot_name;           /* Shortened name of well lease for plotting */
    GSString_t short_name;          /* Shortened or informal name of well */
    
    GSString_t operator;            /* Name of well operator */
    GSString_t licensee;            /* Licensee of well lease */
    GSString_t agent;               /* Name of agent */
    
    /* Class and status information */
    GSString_t  class ;             /* Well classification (e.g. LAHEE class.) */
    GSString_t crstatus;            /* Current well status */
    GSString_t orstatus;            /* Original well status */
    GSString_t prstatus;            /* Previous well status */
    
    /* Location information */
    GSMapLocation_t tophole_xy;     /* structure defining coordinates of 
                                    top hole position */
    GSLegal_t tophole;              /* structure describing legal status of the 
                                     tophole */
    GSMapLocation_t bottomhole_xy;  /* structure defining coordinates of 
                                    bottom hole position */
    GSLegal_t bottomhole;           /* structure describing legal status of the  
                                     bottomhole */
    
    struct 
        {                           /* structure defining offshore specific wells */
        GSMeasurement_t water_depth;    /* Water depth */
        GSString_t platform_name;   /* Name of offshore drilling platform */
        } offshore;
    
    /* Total depth information */
    struct 
        {                           /* structure defining depths of driller, 
                                       logger, and plugback */
        GSDepth_t driller;          /* structure describing driller depths */
        GSDepth_t logger;           /* structure describing logger depths */
        GSDepth_t plugback;         /* structure describing plugback depths */
        GSLithoStrat_t formation;   /* structure describing formation at TD */
        GSTemperature_t temperature;    /* bottom hole temperature */
        GSMeasurement_t pressure;   /* bottom hole pressure */
        } td;
    
    /* Elevation information */
    GSElevationRef_t elevation;     /* Datum elevation for well */
    GSElevation_t Ground_Elevation; /* ground elevation */
    GSElevation_t kb_elevation;     /* kelly bushing elevation */
    GSElevation_t cf_elevation;     /* casing flange elevation */
    
    /* Checkshot information */
    struct 
        {
        GSElevationRef_t datum;     /* datum used for checkshot */
        GSVelocity_t vsd;           /* velocity surface to datum */
        GSUnit_t time_unit;         /* Unit of time in vector */
        GSVector_t depth;           /* measured depth */
        GSVector_t time;            /* two-way travel time */
        } checkshot;
    struct                          /* Drilling information */
        {
        GSString_t contractor;      /* name of drilling contractor */
        GSString_t rig_number;      /* drilling rig number or identifier */
        } drilling;
    
    /* Miscellaneous Information */
    GSString_t primary_source;      /* Name of source of well information */
    GSString_t stratcolumn;         /* name of strat column to use for this well */
    enum 
        {
        GS_IMPERIAL_UNIT,
        GS_METRIC_UNIT
        } original_unit;            /* Original unit system that data was 
                                    collected in */
    GSBoolean_t proprietary;        /* TRUE if this well contains proprietary 
                                     informaiton */
    GSBoolean_t discovery_well;     /* TRUE if this is the discovery field for the field*/
    GSBoolean_t fault;              /* TRUE if this well has a faulted section */
    
    /* Confidentiality Information */
	struct {
		GSBoolean_t flag;           /* confidentiality flag */
		GSString_t formcode;        /* confidentiality formation */
		GSDepth_t depth;            /* confidentiality depth */
		GSDate_t release_date;      /* confidential release date */
		} confidential;
    
    /* Dates */
    GSDate_t Last_Update;           /* date/time of last update */
    GSDate_t Spud_Date;             /* spud date */
    GSDate_t Comp_Date;             /* completion date */
    GSDate_t rigrel;                /* rig release date */
    GSDate_t onprod;                /* on production date */
    GSDate_t calc_onprod;           /* calculated on production date */
    GSDate_t oninject;              /* on injection date */
    GSDate_t status_date;           /* date last status was entered */
    GSDate_t fin_drill;             /* final drilling date */
    
    GSBoolean_t updated;            /* TRUE if this well structure has been updated */
    
    /* Headers (tree-tops) for other well data classes */
    GSVector_t log_run;             /* vector of GSLogRun_t */
    GSVector_t well_core;           /* Vector of core data (WellCore_t) */
    GSVector_t zone_value;          /* Vector of zone values (GSZoneValue_t) */
    GSVector_t well_marker;         /* Vector of GSWellMarker_t */
    GSVector_t well_production;     /* Vector of GSWellProduction_t
                                     (sorted descending by year) */
    GSVector_t well_tubulars;       /* Vector of tubular products placed in well */
    GSVector_t well_remarks;        /* Vector of general remarks */
    } GSWellHdr_t;

/************************************************************************************/
/*                                                                                  */
/*                             WELL CORE STRUCTURE                                  */
/*                                                                                  */
/************************************************************************************/

typedef struct 
    {
    enum 
        {
        GS_WELL_CORE_NORMAL,        /* Normal well core */
        GS_WELL_CORE_SIDEWALL       /* Sidewall core */
        } core_type;
    GSDepth_t top;                  /* Top of cored interval */
    GSDepth_t base;                 /* base of cored interval (not used for sidewall core) */
    GSReal_t recovered;             /* Amount of core recovered, as a percentage (0-100) */
    GSString_t remarks;
    GSString_t show;                /* Type of petroleum show in core */
    } GSWellCore_t;

/************************************************************************************/
/*                                                                                  */
/*                     Remarks, Miscellaneous well information                      */
/*                                                                                  */
/************************************************************************************/

/* The following catch-all is intended to be used to describe some measurement
or activity observed in a well that is not adequately modelled elsewhere in these data
structures.  The intent id to allow another vendor to display this information somehow
without being concerned about the semantics of the information.  This might be used
to display arbitrary information in a cross-section panel, or in a report. */
typedef struct 
    {
    GSString_t type_name;           /* Type of data being described, or name of data set */
    GSDepth_t depth;                /* Depth at this these observations were made
                                      (Zero implies these observations apply
                                      to the well as a whole) */
    GSVector_t strings;             /* A vector of strings describing some event
                                     or measurement at this depth */
    } GSWellRemarks_t;

/************************************************************************************/
/*                                                                                  */
/*                             LOG RUN AND TRACE STRUCTURE                          */
/*                                                                                  */
/************************************************************************************/

typedef struct 
    {
    GSResistivity_t rm;             /* Mud resistivity */
    GSResistivity_t rmf;            /* Mud filtrate resistivity */
    GSResistivity_t rmc;            /* Mud cake resistivity */
    GSString_t source;              /* Source of resistivity observations */
    } GSMudResistivity_t;

typedef struct 
    {
    GSString_t name;                /* Name or mnemonic of sensor (GR, DT, RHOB, etc) */
    GSString_t source;              /* Source of the digital data associated with this sensor */
    GSLongInt_t version;            /* Version number of this trace - used during editing */
    GSString_t comments;            /* General remarks about the log trace */
    GSUnit_t depth_uom;             /* Unit of depth measurement */
    GSUnit_t trace_uom;             /* Unit of trace value */
    GSReal_t non_null_top;          /* Top of trace data interval (excluding nulls) */
    GSReal_t non_null_base;         /* Base of trace data interval (excluding nulls) */
    GSReal_t min;                   /* Minimum trace value */
    GSReal_t max;                   /* Maximum trace value */
    GSReal_t dinc;                  /* Digitizing increment (0.0 if random) */
    GSVector_t processing_history;  /* Vector of processing history records sorted by date,
                                     most recent listed first */
    GSVector_t depth;               /* Array of depth values (4 or 8 byte floating point values)
                                     Note: this is NOT a Depths_t structure) */
    GSVector_t value;               /* Corresponding array of values.  The number of depths
                                     and values should match (4 or 8 byte floating point values) */
    } GSLogTrace_t;

typedef struct 
    {
    /* A service is also loosely called a logging tool.
    Several services may be used in a single pass */
    GSString_t log_service;         /* Name of log service */
    GSString_t tool_id;             /* Tool identifier/serial number */
    GSLongInt_t sequence;           /* Sequence number of this tool in current tool string */
    GSVector_t log_trace;           /* List of sensors that belong to this service/tool */
    } GSLogService_t;

typedef struct 
    {
    /* A pass is a single Trip of the tool string.
    There is usually more than one pass per run */
    GSLongInt_t pass_number;        /* Logging pass number within current log run */
    GSDate_t pass_date;             /* Date this pass was started */
    GSMeasurement_t top;            /* Top of logged interval */
    GSMeasurement_t base;           /* Base of logged interval */
    GSVector_t log_service;         /* List of services being used during this pass */
    } GSLogPass_t;

typedef struct 
    {
    /* A log run is a visit by a logging company to a drilling site.
    It usually is associated with a single billing cycle */
    GSLongInt_t run_number;         /* Log run number */
    GSString_t logging_company;     /* Name of company providing logging service */
    GSDate_t run_date;              /* Date this log run was started */
    GSString_t logging_unit_number; /* Identification of logging truck or unit */
    GSString_t logging_unit_loc;    /* Location of logging unit (home base) */
    GSString_t recorder;            /* Name of person acting as logging recorder */
    GSString_t witness;             /* Name of company witness to logging activity */
    GSElevationRef_t datum;         /* Elevation used as log run datum */
    struct 
        {
        GSString_t type;            /* type of mud */
        GSMeasurement_t salinity;   /* Mud salinity */
        GSMeasurement_t density;    /* Mud weight */
        GSMeasurement_t viscosity;  /* mud viscosity */
        GSMeasurement_t fluid_loss;
        GSReal_t pH;
        GSMudResistivity_t resistivity;   /* Resistivity of mud */
        GSMudResistivity_t resistivity_bht;   /* Resistivity of mud at bottom hole
                                               temperature */
        } mud_data;
    GSMeasurement_t time_circ_stopped;  /* Time since mud circulation was stopped */
    GSMeasurement_t time_logger_on_bottom;  /* Time the logging tool was at bottom of well */
    GSTemperature_t max_rec_temp;   /* Maximum bottom hole temperature recorded */
    GSVector_t log_pass;
    } GSLogRun_t;

/************************************************************************************/
/*                                                                                  */
/*                           ZONE DEFINITIONS                                       */
/*                                                                                  */
/************************************************************************************/

typedef GSString_t GSZoneName_t;
typedef GSString_t GSZoneVariable_t;

typedef struct 
    {
    GSZoneName_t zone_name;         /* Name of zone being defined */
    GSString_t top_definition;      /* Text describing how top of zone is defined */
    GSString_t base_definition;     /* Text describing how base of zone is defined */
    GSString_t remarks;             /* General remarks about zone */
    } GSZoneDefinition_t;

typedef struct 
    {
    GSZoneName_t zone_name;         /* Name of zone in which the variable is defined.
                                     If null, then the defined variable applies to all zones. */
    GSZoneVariable_t variable_name; /* Name of variable being defined */
    GSString_t algorithm;           /* Description of methodology used to define the variable.
                                     This may be an equation or a reference
                                     to a known algorithm. */
    GSString_t remarks;             /* General remarks about zone variable */
    } GSZoneVariableDefinition_t;

typedef struct 
    {
    GSZoneName_t zone_name;         /* Name of the zone.  If null, then the defined
                                     variable applies to entire well or field. */
    GSZoneVariable_t variable_name; /* Name of variable being measured */
    GSDepth_t top;                  /* Depth to top of measured interval */
    GSDepth_t base;                 /* Depth to base of measured interval */
    GSString_t value;               /* Value of zone variable */
    GSUnit_t unit;                  /* Unit of measurement */
    } GSZoneValue_t;

/************************************************************************************/
/*                                                                                  */
/*                          PRODUCTION HISTORY STRUCTURE                            */
/*                                                                                  */
/************************************************************************************/
typedef struct 
    {
    enum 
        {
        GS_OIL,
        GS_WATER,
        GS_GAS
        } fluid_type;               /* Type of fluid produced */
    GSLongInt_t year;               /* Year of record */
    GSZoneName_t zone_name;         /* Name of the zone. */
    GSReal_t cumulative_prior_years;    /* Sum of fluid produced during prior years */
    GSVector_t prod_month;          /* Vector of GSReal_t containing monthly production,
                                     one per month (1st is January, 2nd is February, etc)*/
    GSUnit_t unit;                  /* Unit of measure used to describe production volumes */
    } GSWellProduction_t;

/************************************************************************************/
/*                                                                                  */
/*                          TUBULAR PRODUCTS STRUCTURE                              */
/*                                                                                  */
/************************************************************************************/
typedef struct 
    {
    GSString_t tubing_type;         /* Type of tubing product */
    GSString_t tubing_size;         /* Size of tubing, as a printable string */
    GSDate_t install_date;          /* Date this tubular product was installed */
    GSDepth_t top;                  /* Top of tubular product */
    GSDepth_t base;                 /* base of tubular product */
    GSMeasurement_t bore;           /* Inside diameter of casing */
    GSMeasurement_t hole_size;      /* Size of hole in which tubular product was installed */
    struct 
        {
        GSDepth_t depth;            /* Depth at which casing shoe was installed */
        GSLithoStrat_t form;        /* Formation in which shoe was set */
        } shoe;
    GSReal_t strength;              /* Axial load required to yield pipe */
    GSString_t steel_spec;          /* Manufacturer's spec under which steel was made */
    GSString_t remarks;
    } GSWellTubulars_t;

/************************************************************************************/
/*                                                                                  */
/*                           WELL MARKER STRUCTURES                                 */
/*                                                                                  */
/************************************************************************************/

typedef enum 
    {
    GS_LITHO_TOP,                   /* Lithostratigraphic (formation) top */
    GS_CONTACT_TOP,                 /* Fluid surface contact (e.g. oil/water) */
    GS_FAULT_POINT,                 /* Piercing point of well through fault plane */
    GS_MARKER_TOP,                  /* Stratigraphic marker - subordinate to
                                       lithostratigraphic pick */
    GS_OTHER_TOP
    } GSMarkerType_t;

typedef struct 
    {                               /* Formation top description */
    GSUWI_t uwi;                    /* Unique well identifier (mandatory) */
    GSLithoStrat_t name;            /* Name of lithostratigraphic (surface set) name (mandatory) */
    GSString_t source;              /* Source of interpretation */
    GSSurfaceSetName_t seismic_horizon; /* Name of seismic surface that this marker is
                                           correlated with */
    GSUnit_t z_uom;                 /* unit of measure (used when part of a surface set) */
    GSDepth_t depth;                /* Coordinates of piercing point */
    GSMeasurement_t fault_gap;      /* For faults, the amount of missing section faulted
                                       out at this location */
    enum 
        {
        GS_NORMAL_SECTION = 0,      /* Paleo up indicators point up */
        GS_INVERTED_SECTION = 1     /* Paleo up indicators inverted */
        } up_direction;
    GSString_t Comment;             /* comments describing nature of pick
                                     (ABSENT, ERODED, NOT PICKED, etc) */
    GSBoolean_t Best_Pick;          /* TRUE if this is best available information */
    char Quality;                   /* Quality code of pick */
    GSDate_t Last_Update;           /* date this interpretation made */
    } GSWellMarker_t;

typedef struct                      /* Used as part of a surface set ... */
    {
    GSUWI_t uwi;                    /* Unique well identifier */
    GSMeasurement_t measurement;    /* Attribute value measured at this point */
    GSString_t source;              /* Source of this interpretation/measurement */
    } GSWellAttribute_t;

/* A vector of the following entities describes the lithostratigraphic
codes that may be sent.  It acts as an assertion list and provides some
structural relationships between 'formations' */

typedef struct 
    {
    GSLithoStrat_t code_name;       /* formation and age codes (NOTE: Age code in this
                                       structure implies the Average of the formation.
                                       The exact age at a well is found in the
                                       GSWellMarker_t structure) */
    GSString_t source;              /* Name of company that defines/uses this code */
    GSSurfaceSetName_t full_name;   /* Full marker/formation (surface set) name */
    GSMarkerType_t marker_type;     /* Type of surface */
    GSLithoStrat_t parent;          /* if this unit is a member of a larger group, then
                                     this field contains the name of the next higher order */
    GSString_t remarks;
    } GSLithostratCodeDesc_t;

/* The following represents a stratigraphic column.  This is a list
of lithostratigraphic codes and their depositional ranking, from youngest
to oldest. This list usually describes a normal depositional sequence
independent of any well */

typedef struct 
    {
    GSString_t name;                /* Name of stratigraphic column */
    GSString_t citation;            /* Journal citation describing sequence */
    GSString_t remarks;             /* */
    GSVector_t lithostrat;          /* Vector of formation and age codes */
    } GSStratigraphicColumn_t;

/************************************************************************************/
/*                                                                                  */
/*                    TOP STRUCTURES FOR EACH DATA TYPE                             */
/*                                                                                  */
/************************************************************************************/

/*    Seismic Survey Information
      --------------------------
 */
/*
All seismic related information is specified in a hierarcical structure.  The
top of the hierarcy is a structure which groups the information according to
surveys.  Surveys may be either 2d groups of line or 3d surveys.  In order to
facilitate memory management, 3d survey data will be organized according to
lines.  The following defines the survey structure:                  */

typedef struct 
    {
    GSString_t survey_name;         /* Name of survey */
    GSString_t survey_desc;         /* Survey description */
    enum                            /* Survey type */
        {
        GS_SURVEY_2D = 2,           /* 2d survey symbol */
        GS_SURVEY_3D = 3            /* 3d survey symbol */
        } survey_type;
    GSElevation_t z_datum;          /* Datum to use in association with elevations */
    GSCartoProjection_t proj;       /* Projection information */
    GSDP_t dp_null;                 /* Null value for floating point numbers */
    GSLongInt_t int_null;           /* Null value for integers */
    GSSeismicLocation3D_t loc;      /* 3d survey specificaton and
                                      location structure (not used for 2-D survey) */
    GSVector_t seismic_line;        /* vector of structures describing lines
                                       element_type = GSV_SEISMIC_LINE */
    } GSSeismicSurvey_t;

/*    Surface set Information
      -----------------------
 */
typedef struct 
    {
    GSSurfaceSetName_t surface_name;    /* Name of surface set */
    GSString_t description;         /* description of the surface */
    GSCartoProjection_t proj;       /* Projection information */
    GSDP_t dp_null;                 /* Null value for floating point numbers */
    GSLongInt_t int_null;           /* Null value for integers */
    GSSurfaceType_t surface_type;   /* Type of surface (Fault, horizon, geologic marker) */
    GSVector_t surface_geometry;    /* List of geometry information for surface */
    } GSSurfaceSet_t;

/*    Field Information
      ----------------
 */

typedef struct 
    {
    GSString_t field_name;          /* name of (oil, gas) field */
    GSString_t field_type;          /* What type of field this is */
    GSUWI_t discovery_well;         /* Unique identifier of the discovery well */
    GSVector_t zone_value;          /* Vector of zone values for the field */
    GSVector_t map_polyline;        /* areal extent of field, as a series of
                                     closed polygons.  NOTE: type = GSV_MAP_POLYLINE */
    GSCartoProjection_t proj;       /* Projection information */
    GSDP_t dp_null;                 /* Null value for floating point numbers */
    GSLongInt_t int_null;           /* Null value for integers */
    GSVector_t well_hdr;            /* vector of structures describing wells */
    } GSField_t;

/*    Well Tie Information  
      --------------------
 */

typedef struct                      /* This structure describes the relationship between
                                       a specific surface in a well and its correlation
                                       with a specifiv seismic line and shot point.
                                       Useful for tying geophysical/geological data in
                                       reservoir simulators */
    {
    GSUWI_t uwi;
    GSLithoStrat_t marker;
    GSSurfaceSet_t seismic_surface;
    GSCartoProjection_t proj;       /* Projection information */
    GSString_t survey_name;         /* (optional) */
    GSString_t line_name;           /* (optional) */
    GSDP_t dp_null;                 /* Null value for floating point numbers */
    GSLongInt_t int_null;           /* Null value for integers */
    GSSeismicDataPoint_t loc;       /* location on seismic line */
    } GSSeismicWellTie_t;

/*    Map Information
      ---------------
 */

typedef struct 
    {
    GSString_t source_name;         /* Name of the data source. A null
                                     value is allowed (e.g. no source) */
    GSCartoProjection_t proj;       /* Projection information */
    GSDP_t dp_null;                 /* Null value for floating point numbers */
    GSLongInt_t int_null;           /* Null value for integers */
    GSString_t local_datum;         /* Datum reference for long., lat. */
    GSUnit_t z_unit;                /* Unit of elevation */
    GSExtents_t extent;             /* Area of interest of the map */
    GSVector_t map_objects;
    } GSMap_t;

/*    Fault Trace (in map view) Information
      -------------------------------------
 */

typedef struct 
    {
    GSElevation_t z_datum;          /* Datum to use in association with elevations */
    GSCartoProjection_t proj;       /* Projection information */
    GSDP_t dp_null;                 /* Null value for floating point numbers */
    GSLongInt_t int_null;           /* Null value for integers */
    GSVector_t fault_surface;       /* Vector of structures of surfaces
                                     having fault traces defined */
    } GSFaultTraceSet_t;

/*    List of formation codes and stratigraphic columns
      -------------------------------------------------
 */
typedef struct 
    {
    GSVector_t lithostrat_code_desc;    /* A list of stratigraphic codes */
    GSVector_t stratigraphic_column;    /* List of stratigraphic columns */
    } GSLithostratCodeList_t;

/*    List of zone definitions and variables
      --------------------------------------
 */
typedef struct 
    {
    GSVector_t zone_definition;     /* List of zone definition */
    GSVector_t zone_variable_definition;    /* List of zone variable definitions */
    } GSZoneList_t;

/************************************************************************************/
/*                                                                                  */
/*                    STATUS STRUCTURE                                              */
/*                                                                                  */
/************************************************************************************/

/*  A status structure for returning to the exporting application information
    about the success/failure of the various functions performed by the receiver.
 */

typedef struct 
    {
    GSLongInt_t status_code;        /* error code */
    GSString_t object_key;          /* One or more object keys, separated by ^,
                                       identifying entity that produced status */
    GSString_t status_description;  /* Status description */
    } GSStatusReturn_t;

typedef struct 
    {
    GSVector_t status_return;
    } GSStatusVector_t;

/************************************************************************************/
/*                                                                                  */
/*                    CONTEXT STRUCTURES                                            */
/*                                                                                  */
/************************************************************************************/

/*  There are two parts to receiver context within which a client must operate,
    environmental in the form of a possible memory limitation, and data in the
    form of things such as project names, pass words, file names, etc.  The
    following structures will be used to reply to a request for context
    information : */

typedef struct 
    {
    GSLongInt_t max_memory;         /* Size of local memory buffer, in bytes */
    GSVector_t context_requirements;    /* data context requirements */
    } GSReceiverContext_t;

/*  This structure is associated with defining the information 
to be sent to to the receiver to establish context. */

typedef struct 
    {
    enum 
        {
        GS_INTEGER_TYPE = 1,
        GS_CHARACTER_TYPE = 2,
        GS_FLOAT_TYPE = 3,
        GS_UNSIGNED_INTEGER = 4
        } type;                     /* Data type of context variable */
    GSLongInt_t max_length;         /* Maximum length of context variable *if string) */
    GSBoolean_t confidential;       /* Flag for whether to display user's input */
    GSString_t prompt;              /* Prompt string to use for requesting context variable */
    } GSContextRequirements_t;

/*  This structure is used to transmit the requested information back to
the exporting process.  There will be one string for each requirement specified
in the previous structure.  Even though an integer(perhaps unsigned) may be
required, it will be sent in ascii representation. */

typedef struct 
    {
    GSVector_t string_array;        /*  array of strings */
    } GSContextInformation_t;

/*   Upon establishment of the context, a token will be returned to the client
     process */

typedef GSLongInt_t GSContextToken_t; /*  tag for context established */

/************************************************************************************/
/*                                                                                  */
/*                    EXECUTIVE STRUCTURES                                          */
/*                                                                                  */
/************************************************************************************/

/*  An executive structure containing context destination information for
    cross check reference to the context establishment information
    structure, and a list which provides reference to all 
    supported data type structures which are to be transmitted.  If context
    has been previously established then the context information structure
    need not be included.  If context has not been previously established
    then the context information structure must be referenced first in the 
    list(if necessary).
*/

/*   This structure allows description of an operation upon which to perform on
     the accompanying data structure */

typedef union 
    {
    enum 
        {
        /*    Interaction is complete, terminate                */
        GS_STOP_SERVER = 1000,      /* Address of structure is NULL */
        /*    Break current context, retain comm handshake      */
        GS_CONTEXT_BREAK = 1001,    /* Address of structure is NULL */
        /*    Return context requirements                       */
        GS_SEND_CONTEXT = 1002,     /* Address of structure is NULL */
        /*    Process everything, send no return status         */
        GS_NO_RETURN = 1003         /* Address of structure is NULL */
        } symbolic;
    GSLongInt_t extension;
    } GSDataDisposition_t;

/*  The following is a list of top-level structures ('objects') that GeoLink defines.
    The list is extendable past the enum set by using the extension portion of the union. */

typedef union 
    {
    enum 
        {
        GS_STRUCT_RECEIVER_SPECS = 1,
        GS_STRUCT_RECEIVER_RQMTS = 2,
        GS_STRUCT_CONTEXT_RQMTS = 3,
        GS_STRUCT_CONTEXT_INFO = 4,
        GS_STRUCT_CONTEXT_TOKEN = 5,
        GS_STRUCT_STATUS_RETURN = 6,
        GS_STRUCT_SEISMIC_SURVEY = 11,
        GS_STRUCT_SURFACE_SET = 12,
        GS_STRUCT_WELL_FIELD = 13,
        GS_STRUCT_WELL_TIE = 14,
        GS_STRUCT_MAP = 15,
        GS_STRUCT_FAULT_TRACE_SET = 16,
        GS_STRUCT_LITHOSTRAT_CODES = 17,
        GS_STRUCT_ZONES = 18
        } symbolic;
    GSLongInt_t extension;
    } GSDataStructType_t;

typedef struct 
    {
    GSDataDisposition_t disposition;
    GSDataStructType_t type;
    GSVersion_t version;
    GSVoidptr_t structure_pointer;
    } GSStructure_t;

typedef struct 
    {
    GSContextToken_t context_token; /* tag for context established */
    GSVector_t gs_structure;        /* vector of structures on which to act */
    } GSExecutive_t;

/************************************************************************************/
/*                                                                                  */
/*                    RECEIVER CAPABILITY STRUCTURES                                */
/*                                                                                  */
/************************************************************************************/

/*  A Receiver information structure is returned to the GeoShare Server
    upon request.  This structure contains the following information:
        a.  Status of Receiver service initialization
        b.  A unique name for the receiver so a client can ask for it by name (if known).
        c.  A descriptive text string to show the user in a list of receivers
        d.  Exploration data structures supported
        e.  Data structure revision level range supported
 */

/*  Support for a given data structure is composed of three parts, an enumerated
    parameter which indicates that it is supported, a mask variable in which
    the receiver indicates which components of the structure it can handle, and 
    a structure indicating the GeoShare version which it supports.  The
    following #define statements provide the component masks: */

/*  Surface Set Component Masks  */

#define GSM_SS_GRID            (1<<0)   /* grid structure is supported */
#define GSM_SS_SEISMIC         (1<<1)   /* seismic interpretation is supported */
#define GSM_SS_XYZ             (1<<2)   /* xyz scatter data are supported */
#define GSM_SS_CONSTANT        (1<<3)   /* constant data are supported */
#define GSM_WELL_MARKER        (1<<4)   /* Well Marker data are supported */
#define GSM_SS_CONTOUR         (1<<5)   /* Contour Information is supported */

/*  Seismic Data Support Masks */

#define GSM_SEISMIC_2D         (1<<0)   /* 2d seismic line input is supported */
#define GSM_SEISMIC_3D         (1<<1)   /* 3d Survey input is supported */
#define GSM_SEISMIC_LINE_HDR   (1<<2)   /* Seismic line header data are supported */
#define GSM_SEISMIC_TRACE      (1<<3)   /* Seismic trace data are supported */
#define GSM_SEISMIC_SURFACE    (1<<4)   /* Seismic surface data are supported */

/*  Well information Component Masks */

#define GSM_WELL_CHECKSHOT     (1<<0)   /* Checkshot information is supported */
#define GSM_WELL_PATH          (1<<1)   /* Path data are supported */
#define GSM_WELL_LOG_RUN       (1<<2)   /* Log run data are supported */
#define GSM_WELL_WELL_CORE     (1<<3)   /* Core data are supported */
#define GSM_WELL_ZONE_VALUE    (1<<4)   /* Zone value data are supported */
#define GSM_WELL_WELL_MARKER   (1<<5)   /* Well marker(tops) data are supported */
#define GSM_WELL_PRODUCTION    (1<<6)   /* Production history data are supported */
#define GSM_WELL_TUBULARS      (1<<7)   /* Tubular information are supported */
#define GSM_WELL_REMARKS       (1<<8)   /* Remark information are supported */

/*  Field information Component Masks */

#define GSM_FIELD_WELLS        (1<<0)   /* Field Well data are supported */
#define GSM_FIELD_BOUNDARY     (1<<1)   /* Field boundary is supported */
#define GSM_FIELD_ZONE_VALUE   (1<<2)   /* Field zone data are supported */

typedef struct 
    {
    GSDataStructType_t data_struct_type;
    GSMask_t components_supported;
    GSVersion_t version;
    } GSDataSupported_t;

typedef struct 
    {
    GSLongInt_t receiver_status;    /* Reserved for future use */
    GSString_t receiver_name;       /* Unique character identifier of receiver
                                       (<=20 characters ) */
    GSString_t banner_string;       /* Your chance to advertise your product!
                                       (<=80 characters ) */
    GSVector_t data_supported;      /* List of types of data the server supports */
    } GSReceiverSpecs_t;

/*  A client application may request information from the GeoShare server about
    which application servers are available for the task the client wishes.  It
    may specify the request in terms of a receiver name or in terms of data
    structures supported.  A vector of the following structures will be passed
    to the GeoShare server as input to making a list of receivers which will 
    satisfy the criteria: */

typedef struct 
    {
    GSVector_t requirement;
    } GSReceiverRqmts_t;

typedef struct 
    {
    enum 
        {
        GS_STRUCT_SUPPORTED = 1,
        GS_NAME = 2
        } spec_type;
    GSString_t receiver_name;
    GSDataSupported_t data;
    } GSRequirement_t;

/************************************************************************************/
/*                                                                                  */
/*                    END OF INCLUDE FILE GEOSCIENCE.H                              */
/*                                                                                  */
/************************************************************************************/

#endif
