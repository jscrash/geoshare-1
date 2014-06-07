#ifndef _GEOSCIENCE_H_DEFINED
#define _GEOSCIENCE_H_DEFINED

/*-----------------------------------------------------------------------------
| 
|   Module:    geoscience.h
|
|-------------------------------------------------------------------------------
| 
|         This code is the product of GeoQuest Systems, Inc. and Finder 
|         Graphics Systems, Inc and is protected under U.S. Copyright laws.
|         This code may be reproduced in part or whole by any
|         technique or facsimile adoption as long as this notice is included.
| 
|-------------------------------------------------------------------------------
|
|   Purpose:   This file contains symbol definitions and structure
|              specifications to be used in data interchange facilities
|              Current working title of system : GeoShare (generic) 
|              Other names: EDICT (GeoQuest Systems)
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
|                               O Osawa, IDS Schlumberger
|                               S Guthery, ASC Schlumberger
|                               H Ganem, ESF Schlumberger
|                               K Waagbo, GECO Schlumberger
|                  Blue Book I
|             21-Mar-1991      S. McAdoo, GQS
|                  An experiment with separating seismic surfaces from the seismic
|                  data to fit jgg's surface set concept.
|             29-Mar-1991      J Gillespie, FGS
|                  Assert the GS naming convention throughout the file
|                  Add usage notes
|              2-Apr-1991      S McAdoo, GQS
|                  change enum with extension union constructs
|              8-Apr-1991      S McAdoo, GQS
|                  work on seismic header constructs
|             16-Apr-1991      J Gillespie, FGS
|                  'Better' normalization of cdp,sp,trace#
|                  Add velocity functions at GSSeismicLineHdr_t level
|             19-Apr-1991      J Gillespie, FGS
|                  Rearrange GSSurfaceSetIntersection logic
|                  (Per memo from H Ganem 8 April 1991)
|             23-Apr-1991      J Gillespie, FGS
|                              O Osawa, IDS Schlumberger
|                  Add cross section description to GSField_t
|             24-Apr-1991      J Gillespie, FGS
|                              S McAdoo, GQS
|                              R Veazey, GQS
|                  Address relationship between map location, seismic shot point,
|                  receiver, cdp (cmp)
|                  Strip out derivable amplitude statistics
|                  First attempt at merging geologic, geophysical sections
|             7-May-1991       J Gillespie, FGS
|                              O Osawa, IDS Schlumberger
|                  Add more units to resolve some ambiguities
|                  Made value in GSZoneValue_t a measurement instead of a string
|                  Added reshot logic to line-level shot points (still need to
|                      consider adding reshot character to GSShotPoint_t)
|     0.9     14-May-1991      J Gillespie, FGS
|                  Add GSLegalLoc_t, GSLandNet_t structures to describe land net
|                  Change GSLegal_t definition (well_hdr.tophole and
|                  well_hdr.bottomhole are now of type GSLegalLoc_t)
|     0.91    15-May-1991      J Gillespie, FGS Schlumberger
|                              O Osawa, IDS Schlumberger
|                              S McAdoo, GQS
|                              O Lindtjorn, GECO Schlumberger
|                  Rectify GSV omissions, missspellings in last delta
|                  Change mixed case structure element names to be lower case
|                  Add operation vector to GSDataSupported_t
|                  Added GSXYZOffset_t, GSLocation_t to model 'uncertainty' of location
|     1.00    20-May-1991      J Gillespie, FGS Schlumberger
|                  First 'official' release
|                  Moved documentation to geoscience.doc
|
+-----------------------------------------------------------------------------*/

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
typedef long GSMask_t;              /* Bit mask, used to identify item validity in
                                       structures */

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

typedef enum 
    {
    GSV_UNKNOWN = 0,                /* vector type is unknown, check extension */
    /* basic data types */
    GSV_INT = 110,                  /* vector type GSLongInt_t */
    GSV_REAL = 111,                 /* vector type GSReal_t */
    GSV_DP = 112,                   /* vector type GSDP_t */
    GSV_FRAC8 = 113,                /* vector type 8 bit int fraction */
    GSV_FRAC16 = 114,               /* vector type 16 bit int fraction */
    GSV_UNSIGNED_LONG = 115,        /* vector type GSUlong_t */
    GSV_BOOLEAN = 116,              /* vector type GSBoolean_t */
    GSV_RATIONAL = 117,             /* vector type GSRational_t */
    GSV_CHAR = 118,                 /* vector type char */
    GSV_COMPLEX = 119,              /* vector type complex */
    GSV_STRING = 120,               /* vector type GSString_t */
    /* executive structure vectors */
    GSV_RECEIVER_RQMTS = 200,       /* vector type GSReceiverRqmts_t */
    GSV_DATA_SUPPORTED = 201,       /* vector type GSDataSupported_t */
    GSV_GSSTRUCTURE = 202,          /* vector type GSStructure_t */
    GSV_RECEIVER_CONTEXT = 203,     /* vector type GSReceiverContext_t */
    GSV_STATUS_RETURN = 204,        /* vector type GSStatusReturn_t */
    /* basic point structures */
    GSV_XYZ_POINT = 1000,           /* vector type GSXYZPoint_t */
    GSV_SEISMIC_DATA_POINT = 1001,  /* vector type GSSeismicDataPoint_t */
    GSV_MAP_LOCATION = 1002,        /* vector type GSMapLocation_t */
    GSV_DEPTH = 1003,               /* vector type GSDepth_t */
    GSV_PROCESSING_HISTORY = 1004,  /* vector type GSProcessingHistory_t; */
    GSV_NODE_ID = 1005,             /* vector type GSNodeID_t */
    /* stratigraphic data structures */
    GSV_STRAT_COLUMN = 2000,        /* vector type GSStratigraphicColumn_t */
    GSV_LITHOSTRAT_CODE_DESC = 2001,    /* vector type GSLithostratCodeDesc_t */
    /* zone structures */
    GSV_ZONE_VARIABLE_DEF = 3000,   /* vector type GSZoneVariableDefinition_t */
    GSV_ZONE_DEF = 3001,            /* vector type GSZoneDefinition_t */
    GSV_ZONE_VALUE = 3002,          /* vector type GSZoneValue_t */
    /* well structures */
    GSV_WELL_REMARKS = 4001,        /* vector type GSWellRemarks_t */
    GSV_WELL_TUBULARS = 4002,       /* vector type GSWellTubulars_t */
    GSV_WELL_PRODUCTION = 4003,     /* vector type GSWellProduction_t */
    GSV_WELL_MARKER = 4004,         /* vector type GSWellMarker_t */
    GSV_WELL_CORE = 4005,           /* vector type GSWellCore_t */
    GSV_WELL_HDR = 4006,            /* vector type GSWellHdr_t */
    GSV_WELL_TEST_HDR = 4007,       /* vector type GSWellTestHdr_t */
    GSV_SEISMIC_WELL_TIE = 4008,    /* vector type GSSeismicWellTie_t */
    /* well log structures */
    GSV_LOG_RUN = 5000,             /* vector type GSLogRun_t */
    GSV_LOG_PASS = 5001,            /* vector type GSLogPass_t */
    GSV_LOG_SERVICE = 5002,         /* vector type GSLogService_t */
    GSV_LOG_TRACE = 5003,           /* vector type GSLogTrace_t */
    /* seismic data structures */
    GSV_SEISMIC_TRACE = 6000,       /* vector type GSSeismicTrace_t */
    GSV_SEISMIC_CLASS = 6001,       /* vector type GSSeismicClass_t */
    GSV_SEISMIC_LINE = 6002,        /* vector type GSSeismicLine_t */
    GSV_SEISMIC_LOC_INFO = 6003,    /* vector type GSSeismicLocInfo_t */
    GSV_SEISMIC_SHOT_POINT_LOC = 6004,  /* vector type GSSeismicShotPointLoc_t */
    GSV_SHOT_POINT = 6005,          /* vector type GSShotPoint_t */
    GSV_SHOT_POINT_SEGMENT = 6006,  /* vector type GSShotPointSegment_t */
    GSV_SEISMIC_VELOCITY = 6007,    /* vector type GSSeismicVelocity_t */
    GSV_VELOCITY_PAIR = 6008,       /* vector type GSVelocityPair_t */
    /* map object structures */
    GSV_MAP_POLYLINE = 7000,        /* vector type GSMapPolyline_t */
    GSV_MAP_POINT = 7001,           /* vector type GSMapPoint_t */
    GSV_MAP_SYMBOL = 7002,          /* vector type GSMapSymbol_t */
    GSV_MAP_ANNOTATION = 7003,      /* vector type GSMapAttribute_t */
    GSV_MAP_ATTRIBUTE = 7004,       /* vector type GSMapAnnotation_t */
    GSV_MAP_OBJECTS = 7005,         /* vector type GSMapObjects_t */
    GSV_ATTRIBUTE_ITEM = 7006,      /* vector type GSAttributeItem_t */
    GSV_LAND_NET = 7007,            /* vector type GSLandNet_t */
    /* seismic interpretation surface structures */
    GSV_CONTACT = 8000,             /* vector type GSContact_t */
    GSV_SEGMENT = 8001,             /* vector type GSSegment_t */
    /* fault trace structures */
    GSV_FAULT_TRACE = 9000,         /* vector type GSFaultTrace_t */
    GSV_FAULT_SURFACE = 9001,       /* vector type GSFaultSurface_t */
    /* surface executive structures */
    GSV_SURFACE_ATTRIBUTE = 10000,  /* vector type GSSurfaceAttribute_t */
    GSV_SURFACE_GEOMETRY = 10001,   /* vector type GSSurfaceGeometry_t */
    GSV_SURFACE_REPRESENTATION = 10002, /* vector type GSSurfaceRepresentation_t */
    /* seismic interpretation surface structures */
    GSV_SEISMIC_LINE_INTERSECTION = 11000,  /* vector type GSSeismicLineIntersection_t */
    GSV_SURFACE_SURFACE_INTERSECTION = 11001,   /* vector type GSSurfaceSurfaceIntersection_t */
    GSV_SURFACE_TRACE = 11002,      /* vector type GSSurfaceTrace_t */
    /* section structures */
    GSV_GEOL_CROSS_SECTION = 12000, /* vector type GSGeolCrossSection_t */
    GSV_SECTION_INTERSECTION = 12001/* vector type GSSectionIntersection_t */
    } GSVectorType_t;

typedef struct 
    {
    GSVectorType_t element_format;
    GSLongInt_t extension;
    GSUlong_t element_size;
    GSUlong_t number_elements;
    GSVoidptr_t vector_pointer;
    } GSVector_t;

/*  ALL strings will be stored as vectors,  the typedef GSString_t is defined as a
    convenience and a vector type differentiater */

typedef GSVector_t GSString_t;

typedef GSString_t GSDate_t;        /* Standard date format is YYYYMMDDHHMNNSS.SS
                                       YYYY from 0000 to 9999 (year)
                                       MM from 01 to 12 (month of year)
                                       DD from 01 to 31 (day of month)
                                       HH from 00 to 23 (hour of day)
                                       MN from 00 to 59 (minute of hour)
                                       SS.SS from 0 to 59.99 (seconds in minute)
                                       Any component may be truncated
                                       (e.g.April 1987 is entered as 198704) */

/************************************************************************************/
/*                                                                                  */
/*                           NULL VALUES STRUCTURE                                  */
/*                                                                                  */
/************************************************************************************/

typedef struct 
    {
    GSDP_t v_dp;                    /* double precision floating point null value */
    GSReal_t v_real;                /* single precision floating point null value */
    GSRational_t v_rational;        /* rational number null value */
    GSLongInt_t v_longint;          /* long integer null value */
    GSUlong_t v_ulong;              /* Unsigned long integer null value */
    } GSNullValue_t;

/************************************************************************************/
/*                                                                                  */
/*                    Data Structure Typedefs                                       */
/*                                                                                  */
/************************************************************************************/

typedef GSLongInt_t GSNodeID_t;     /* node identifier */
typedef GSString_t GSPlotSymbol_t;  /* Graphics plot symbol name */
typedef GSString_t GSUWI_t;         /* Unique Well Identifier */

typedef GSString_t GSSurfaceSetName_t;  /* Name of a surface set */
typedef GSString_t GSSeismicSurveyName_t;   /* name of a seismic survey */
typedef GSString_t GSSeismicLineName_t; /* name of a seismic line in a survey */

/* 
 *       Geophysical data types 
 */

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
    GS_SRF_TYPE_HOR = 1,            /* surface type 'horizon' (lithologic or seismic) */
    GS_SRF_TYPE_FLT = 2             /* surface type 'fault' (or other discontinuity) */
    } GSSurfaceType_t;

typedef enum                        /* z value code */
    {
    GS_Z_VALUE_UNDEFINED = 0,       /* no z value is defined */
    GS_Z_VALUE_TIME = 1,            /* z value is time */
    GS_Z_VALUE_UNSNAPPED_TIME = 2,  /* z value is unsnapped time */
    GS_Z_VALUE_AMPLITUDE = 3,       /* z value is amplitude */
    GS_Z_VALUE_DEPTH = 4            /* z value is depth */
    } GSZValueCode_t;

typedef enum 
    {
    GS_SNAP_P_NEAR = 1,             /* nearest peak to center of window */
    GS_SNAP_P_MAX = 2,              /* largest peak in window           */
    GS_SNAP_T_NEAR = 3,             /* nearest trough to center of window */
    GS_SNAP_T_MAX = 4,              /* largest trough in window         */
    GS_SNAP_ZPN = 5,                /* zero crossing, positive to negative*/
    GS_SNAP_ZNP = 6,                /* zero crossing, negative to positive*/
    GS_SNAP_NULL = 7,               /* snap to input point */
    GS_SNAP_UNDEFINED = 8           /* undefined snap type */
    } GSSnapEvent_t;
/* 
 *      Seismic Line type 
 */

typedef enum 
    {
    GS_LINE_2D = 30,                /* line type 2d */
    GS_LINE_3DI = 31,               /* line type 3d inline */
    GS_LINE_XLN = 32,               /* line type 3d cross line */
    GS_LINE_RANDOM = 33             /* line type 3d random (reconstruction) cut */
    } GSLineType_t;

/*    
 *       Processing History structure           
 */
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
    } GSPolylineType_t;

/*
 *       compass points (more added by concatenating digits, e.g. NNE = 112)
 */

typedef enum 
    {
    GS_NORTH = 1,
    GS_NORTHEAST = 12,
    GS_EAST = 2,
    GS_SOUTHEAST = 42,
    GS_SOUTH = 4,
    GS_SOUTHWEST = 48,
    GS_WEST = 8,
    GS_NORTHWEST = 18
    } GSCompassPoint_t;             /* enum for compass points */

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
    GSString_t type;                /* DISTANCE, TIME, MASS, TEMPERATURE, etc */
    GSString_t unit;                /* Unit (ft, s) or expression (g/cm3) */
    } GSUnit_t;                     /* NOTE: see geoscience.doc for list of units */

typedef struct 
    {
    GSDP_t value;                   /* value of measurement */
    GSUnit_t uom;                   /* unit of measure, including the
                                       measurement system and the actual unit */
    } GSMeasurement_t;

/* The following are special cases of GSMeasurement_t as noted */
typedef GSMeasurement_t GSTemperature_t;    /* uom.type = "TEMPERATURE" */
typedef GSMeasurement_t GSResistivity_t;    /* uom.type = "RESISTIVITY" */
typedef GSMeasurement_t GSTime_t;   /* uom.type = "TIME" */
typedef GSMeasurement_t GSVelocity_t;   /* uom.type = "VELOCITY" */
typedef GSMeasurement_t GSMass_t;   /* uom.type = "MASS" */
typedef GSMeasurement_t GSDistance_t;   /* uom.type = "DISTANCE"\and implication
                                           that measurement is made in horizontal sense */
typedef GSMeasurement_t GSElevation_t;  /* uom.type = "DISTANCE" and implication
                                           that measurement is made in vertical sense */

typedef struct 
    {                               /* structure describing reference elevation */
    GSElevation_t val;              /* Elevation to use for subsea corrections */
    GSString_t ref;                 /* Data type being used as elevation reference
                                       (KB, DF, GS, SL, etc) */
    } GSElevationRef_t;

/************************************************************************************/
/*                                                                                  */
/*                   MASKS DEFINING VALIDITY OF ITEMS                               */
/*                                                                                  */
/************************************************************************************/

/* Used in point structures */
#define GSM_VALID_CDP          (1<<0)   /* cdp number is valid */
#define GSM_VALID_SP           (1<<1)   /* shot point number is valid */
#define GSM_VALID_X            (1<<2)   /* x location is valid */
#define GSM_VALID_Y            (1<<3)   /* y location is valid */
#define GSM_VALID_Z            (1<<4)   /* z location is valid */
#define GSM_VALID_LONGITUDE    (1<<5)   /* longitude is valid */
#define GSM_VALID_LATITUDE     (1<<6)   /* latitude is valid */
#define GSM_VALID_CONNECT      (1<<7)   /* connectivity vector in xyz scatter data */
#define GSM_VALID_NODE         (1<<8)   /* node identifier is valid */

/* Used in GSDepth_t structure */
#define GSM_VALID_MD           (1<<9)   /* measured depth is valid */
#define GSM_VALID_SUBDATUM     (1<<10)  /* subdatum depth is valid */
#define GSM_VALID_TVD          (1<<11)  /* true vertical depth is valid */
#define GSM_VALID_TIME         (1<<12)  /* time is valid */
#define GSM_VALID_DX           (1<<13)  /* deviation in x direction is valid */
#define GSM_VALID_DY           (1<<14)  /* deviation in y direction is valid */

/* Used in GSSeismicLocInfo_t structure */
#define GSM_VALID_DATUM        (1<<15)  /* subdatum depth is valid */
#define GSM_VALID_WDEPTH       (1<<16)  /* weathering depth is valid */
#define GSM_VALID_WVEL         (1<<17)  /* weathering velocity is valid */
#define GSM_VALID_WSTATIC      (1<<18)  /* weathering static is valid */
#define GSM_VALID_DSTATIC      (1<<19)  /* datum static is valid */
#define GSM_VALID_ASTATIC      (1<<20)  /* automatic static is valid */

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
    GSMask_t valid;                 /* Validity mask for XYZpoint:
                                      GSM_VALID_X, GSM_VALID_Y, GSM_VALID_Z */
    GSDP_t x;                       /* x coordinate value associated with point */
    GSDP_t y;                       /* y coordinate value associated with point */
    GSDP_t z;                       /* z coordinate value associated with point */
    } GSXYZPoint_t;

typedef struct 
    {
    GSNodeID_t node_id;             /* Node id */
    GSMask_t valid;                 /* Validity mask for map location:
                                      GSM_VALID_LONGITUDE, GSM_VALID_LATITUDE */
    GSDP_t longitude;               /* Longitude of map location, in decimal degrees */
    GSDP_t latitude;                /* Latitude of map location, in decimal degrees */
    GSXYZPoint_t coord;             /* cartesian coordinates associated
                                       with map location */
    } GSMapLocation_t;

typedef struct 
    {
    GSMask_t valid;                 /* Validity mask for offset:
                                      GSM_VALID_X, GSM_VALID_Y, GSM_VALID_Z */
    enum 
        {
        GS_OFFSET_ABSOLUTE,         /* offset is an absolute offset from map location */
        GS_OFFSET_UNCERTAINTY       /* offset is an uncertainty window around map location */
        } offset_type;
    /* all offsets are in the projection coordinate system */
    GSDP_t x;                       /* x offset/window associated with point */
    GSDP_t y;                       /* y offset/window associated with point */
    GSDP_t z;                       /* z offset/window associated with point */
    } GSXYZOffset_t;

typedef struct 
    {
    GSNodeID_t node_id;             /* Node id */
    GSXYZOffset_t offset;           /* offset from node's map location */
    } GSLocation_t;

typedef struct 
    {
    GSNodeID_t node_id;             /* Map location identifier */
    GSMask_t valid;                 /* Validity mask for station vector:
                                        GSM_VALID_CDP, GSM_VALID_SP */
    GSCDP_t cdp;                    /* common depth point number */
    union 
        {
        GSShotPoint_t shot_point;   /* Shot Point Number associated with point (usually 2d) */
        GSLineNumber_t line_number; /* seismic line number (Used for time slices
                                       and random sections) */
        } y;
    } GSSeismicDataPoint_t;

typedef struct 
    {
    GSNodeID_t node_id;             /* Map location identifier */
    GSShotPoint_t shot_point;       /* Shot Point Number associated with point (usually 2d) */
    GSXYZOffset_t offset;           /* Offset/uncertainty of seismic location */
    } GSSeismicShotPointLoc_t;

typedef struct 
    {
    GSString_t reshoot;             /* reshoot segment identifier - normally null
                                      (when used, usually single character) */
    GSVector_t seismic_shot_point_loc;
    } GSSeismicShotPointSegment_t;

/*    Shot/receiver/cdp information table
      ----------------------------------- */

typedef struct                      /* for shot/receiver/cdp */
    {
    GSNodeID_t node_id;             /* Map location identifier */
    GSMask_t valid;                 /* Validity mask for qualifiers */
    GSElevation_t datum;            /* elevation datum (used only if this line has a
                                       floating datum) */
    GSElevation_t wdepth;           /* depth of weathering layer */
    GSVelocity_t wvel;              /* velocity of weathering layer */
    GSStatic_t wstatic;             /* weathering static */
    GSStatic_t dstatic;             /* datum static */
    GSStatic_t astatic;             /* automatic static */
    } GSSeismicLocInfo_t;

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
 
We will assume all location information contained within a data
structure is given in the same coordinate system.  To properly define the
coordinate system the following structure is used to specify the
projection.  The structures and projection codes are based on the public domain
projection package from the United States Geological Survey (USGS).
(see geoscience.doc for a list of valid parameters) */

/************************************************************************/

#define GS_NO_OF_PROJECTIONS (GSLongInt_t)21
#define GS_NO_OF_SPHEROIDS   (GSLongInt_t)20
#define GS_NO_OF_UNITS       (GSLongInt_t)5

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
    GSSurfaceSetName_t surface_set_name;    /* Surface set name */
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
    GSVector_t fault_surface;       /* Vector of structures of surfaces
                                       having fault traces defined */
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
    /* For the following two vectors, element_format = GSV_REAL;
       element_size = either sizeof (GSReal_t) or sizeof (GSDP_t) */
    GSVector_t xvals;               /* x-values corresponding to z-values */
    GSVector_t yvals;               /* y-values corresponding to z-values */
    } GSGrid_t;

/*    Contour Information
      -------------------
 */

typedef struct 
    {
    GSSurfaceSetName_t surface_set_name;    /* Surface set name */
    GSString_t attribute;           /* name of the attribute being modelled */
    GSUnit_t z_unit;
    GSVector_t fault_surface;       /* Vector of structures of surfaces
                                       having fault traces defined that were used
                                       to generate this contoured surface set */
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
    /* grid defines the geometry representation of the 3d survey,
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
    /* Enter in a clockwise sense; DO NOT assume grid is orthonormal
 (the map_coord node identifiers should correspond to the node identifiers
 in the local_coord array) */
    GSSeismicDataPoint_t local_coord[4];
    GSMapLocation_t map_coord[4];
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

/*    Processed Seismic Line Header
      ----------------------------- */
typedef struct 
    {
    enum 
        {
        GS_NO_MIGRATION = 0,
        GS_PRESTACK_MIGRATION = 1,
        GS_POSTSTACK_MIGRATION = 2
        } migration_type;
    
    enum 
        {
        GS_TRACE_ORDER_SHOT = 1,    /* shot ordered */
        GS_TRACE_ORDER_CDP = 2,     /* unstacked cdps */
        GS_TRACE_ORDER_STACK = 3,   /* stacked cdps */
        GS_TRACE_ORDER_SEQ = 4      /* trace sequential */
        } trace_order;              /* Trace ordering */
    
    GSBoolean_t offset_validity;    /* TRUE = offsets are valid */
    
    GSDistance_t trail_offset;      /* trailing trace offset */
    GSDistance_t lead_offset;       /* leading trace offset */
    GSDistance_t near_offset;       /* ABS near trace offset */
    GSDistance_t far_offset;        /* ABS far trace offset */
    
    GSReal_t sample_interval;       /* sampling rate */
    GSLongInt_t n_samples;          /* maximum number of samples per trace */
    
    GSLongInt_t n_channels;         /* number of channels per record  (1) */
    GSLongInt_t n_fold;             /* stack fold (no. of traces per cdp) */
    
    GSShotPoint_t min_shotpoint;    /* minimum shotpoint */
    GSShotPoint_t max_shotpoint;    /* maximum shotpoint */
    GSRational_t min_receiver;      /* minimum receiver location */
    GSRational_t max_receiver;      /* maximum receiver location */
    GSCDP_t min_cdp;                /* minimum cdp */
    GSCDP_t max_cdp;                /* maximum cdp */
    
    GSRational_t cdp_incr;          /* cdp number increment per trace (zero if invalid) */
    GSRational_t shotpoint_incr;    /* shotpoint increment per trace (zero if invalid) */
    
    } GSSeismicLineHdr_t;


/*    Velocity structures
      -------------------- */
typedef struct 
    {
    GSReal_t z;                     /* time or depth */
    GSReal_t value;                 /* velocity measurement at this time or depth */
    } GSVelocityPair_t;

typedef struct 
    {
    enum 
        {
        GS_STACKING_VELOCITY = 1,
        GS_AVERAGE_VELOCITY = 2,
        GS_PSEUDO_VELOCITY = 3,
        GS_INTERVAL_VELOCITY = 4
        } velocity_type;
    GSUnit_t z_unit;
    GSUnit_t value_unit;
    GSVector_t velocity_pair;       /* List of time/velocity or depth/velocity pairs */
    } GSSeismicVelocity_t;

/*    Trace (including header)
      ------------------------ */

typedef struct 
    {
    /**** trace header ****/
    enum 
        {
        GS_DEAD_TRACE = -1,
        GS_LIVE_TRACE = 1,
        GS_SWEEP_TRACE = 2,
        GS_NULL_TRACE = 3
        } trace_type;               /* activity of trace (live, dead, etc) */
    
    GSLongInt_t record_no;          /* sequential record number */
    GSLongInt_t record_trace_no;    /* record trace number */
    GSLongInt_t field_record_no;    /* field record number */
    GSLongInt_t field_record_trace_no;
    
    GSCDP_t cdp;                    /* common midpoint number (cmp) */
    GSLongInt_t trace_no;           /* trace number corresponding to this cdp */
    GSShotPoint_t shot_point;       /* shotpoint corresponding to this cdp */
    GSShotPoint_t receiver;         /* receiver corresponding to this cdp */
    GSLongInt_t fold;               /* stacking fold at this cdp */
    GSNodeID_t node_id;             /* reference to the cdp location */
    
    GSLongInt_t mute_length;        /* length of initial mute is samples */
    GSReal_t tfs;                   /* time of first sample in sample units */
    GSStatic_t bulk_static;         /* bulk static */
    GSStatic_t trim_static;         /* non-surface consistent automatic static */
    
    GSReal_t minimum_scalar;        /* scalar values used in scaling data to */
    GSReal_t maximum_scalar;        /* fraction values */
    
    GSVector_t seismic_velocity;    /* vector of seismic velocity functions */
    
    /**** trace samples ****/
    
    GSVector_t samples;             /* Vector of seismic trace samples
                                     (see vector descriptor for sample format) -
                                    (Note: no corresponding GSSamples_t exists) */
    } GSSeismicTrace_t;

/*    Line Description Structure
      --------------------------
                                                                                                                                                 
All surveys are organized as a series of lines and the survey structure
includes a vector of line structures of the following form.  Each line may have
several variants usually depending on any processes which have been applied to
the data to alter its presentation or to extract some quality of interest. 
Therefore the line structure contains location information and a vector of
classes.  There may be many types of data related to a line class.  Each type 
will have a structure (or vector of structures) in which that type of data 
is stored.  Included in the current proposal is a vector of interpreted 
surfaces and seismic trace data. */

typedef struct 
    {
    GSLineType_t line_type;         /* Seismic Line type */
    GSSeismicLineName_t line_name;  /* 2d or reconstruction cut line name */
    GSLongInt_t line_number;        /* Line number (required for GS_LINE_3DI, GS_LINE_XLN) */
    GSString_t line_desc;           /* Line description */
    GSVector_t map_location;        /* array of map locations referred to in this line */
    GSVector_t seismic_loc_info;    /* array of datae, weathering info and statics */
    GSVector_t seismic_shot_point_segment;  /* array  of GSSeismicShotPointSegment_t */
    GSVector_t seismic_receiver;    /* array  of GSSeismicShotPointSegment_t
                                       GS_LINE_2D, GS_LINE_RANDOM types only */
    GSVector_t seismic_class;       /* Vector of seismic line variants delineated
                                       according to class */
    } GSSeismicLine_t;

/*    GSSeismicClass_t structure                
      -------------------------- */

typedef struct 
    {
    GSString_t class_name;          /* User assigned class name (e.g. migrated, pre-stack) */
    GSString_t class_desc;          /* Class description */
    GSUnit_t z_unit;                /* unit of measure for time/depth */
    GSSeismicLineHdr_t seismic_line_hdr;    /* Structure containing seismic line header */
    GSVector_t seismic_trace;       /* Vector of GSSeismicTrace_t */
    GSVector_t seismic_line_intersection;   /* Vector of GSSeismicLineIntersection_t */
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
values at which the horizon occurred.  He may have also saved the seismic trace
amplitude at that time.  Segments containing both time and containing amplitude
may be stored for a given surface. */

/*    Section Intersection
      --------------------
      Note: this object describes the intersection between one
            surface set and one segmented traverse section

*/

typedef struct 
    {
    GSSurfaceSetName_t surface_set_name;    /* Surface Set Name */
    GSSurfaceType_t surface_type;   /* Surface type:
                                       GS_SURF_TYPE_HOR or GS_SURF_TYPE_FLT */
    GSString_t surface_desc;        /* Surface description */
    GSString_t source;              /* Name of interpreter or source of data */
    GSDate_t modified_date;         /* date this surface was last modified */
    GSUnit_t x_unit;                /* Unit for x measurements along section */
    GSUnit_t z_unit;                /* Unit for z_value */
    GSVector_t x_value;             /* X distance of each point from the first
                                       point of the section along the section line
                                       (GSReal_t) */
    GSVector_t z_value;             /* depth value of each point (GSReal_t) */
    GSVector_t connect;             /* Connectivity vector; the same sense
                                       as used in GSXYZData_t
                                       (4 byte integer) */
    GSVector_t up_zone_number;      /* Zone sequential number (0,1, 2, ...) of
                                       the zone above each point.  The number
                                       is in the order of the appearance in
                                       zone_name field in GSGeolCrossSection_t
                                       (4 byte integer) */
    GSVector_t down_zone_number;    /* Zone sequential number of the zone
                                       below each point (see above) */
    } GSSectionIntersection_t;

/*  This structure contains information specifying points along the surface at
    which the user has indicated that it intersects another surface.  The structure
    includes the location of the intersection and information about the
    intersecting structure. */

typedef struct 
    {
    GSSectionIntersection_t section_intersection;
    GSSeismicLineName_t line_name;  /* seismic line name */
    
    /* The following are applicable only to horizon surfaces */
    GSSnapEvent_t snap_event;
    GSReal_t snap_window;           /* Duration of snapping window in time units */
    GSVector_t segment;             /* Vector of interpretation segments for line */
    GSVector_t contact;             /* Vector of contact points associated with this
                                     surface on this line */
    } GSSeismicLineIntersection_t;

typedef struct 
    {
    GSSeismicDataPoint_t contact_loc;   /* location of contact point */
    GSZValueCode_t z_value_code;    /* nature of Z value in contact_loc */
    enum 
        {
        GS_FAULT_CONTACT,           /* contact is at a fault */
        GS_UNCONFORM_CONTACT,       /* one surface deposited on another depositional surface
                                       after erosional events */
        GS_NONCONFORM_CONTACT,      /* one surface deposited on 'basement'
                                       after extensive erosional events */
        GS_ANGULAR_CONTACT          /* one surface deposited on another
                                       after erosional and deformational events */
        } contact_type;             /* nature of contact */
    GSSurfaceType_t surface_type_2; /* Surface type: 
                                      GS_SRF_TYPE_HOR, GS_SRF_TYPE_FLT,
                                      GS_SRF_TYPE_UNKNOWN */
    GSSurfaceSetName_t name_2;      /* second name associated with contact */
    } GSContact_t;

typedef struct 
    {
    GSLongInt_t seg_id;             /* Number identifying segment */
    GSZValueCode_t z_value_code;
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
transmitted separately.  The following structures will be used to contain and
transmit fault trace information.  The top structure contains information
necessary to define the coordinate system of the data contained in the
structure and a vector of surface structures for which fault trace information
is transmitted. If both time and depth values are to be associated with a fault
trace, then the same trace should be sent twice - once with the z_unit set to the TIME
system, once for DEPTH */

/*  Each surface may have intersections with many faults.  The following 
    structure names a vector of fault traces for a specified surface. */

typedef struct 
    {
    GSSurfaceSetName_t surface_set_name;    /* Name of surface for which traces are
                                               defined */
    GSUnit_t z_unit;                /* Unit of measure of z dimension (null if not used) */
    GSVector_t fault_trace;         /* Vector of structures defining fault traces */
    } GSFaultSurface_t;

/*  The following fault trace structure specifies the name of the intersecting fault
    and names the vector containing the points associated with the intersection. */

typedef struct 
    {
    GSSurfaceSetName_t fault_name;  /* Name of fault whose intersection with the surface
                                       this trace describes */
    GSLongInt_t ft_seg_id;          /* segment id for fault trace for a given surface */
    GSPolylineType_t open_closed;
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
        GS_REP_CONTOUR,
        GS_REP_CONSTANT,
        GS_REP_XYZ
        } type;
    union 
        {
        GSGrid_t grid;
        GSXYZData_t xyz;            /* 'Scatter' data file */
        GSMeasurement_t constant;
        GSContour_t contour;
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
                                       which correspond to, or are measured at;
                                       the geometry specified in either time or depth */
    } GSSurfaceGeometry_t;

/********************************************************************
   This structure describe the intersection between a surface set and 
   another surface set. It is a structure that simply copies the
   GSFaultTraceSet_t structure. It should be more general and cover also
   the faults
 ********************************************************************/

typedef struct 
    {
    GSZValueCode_t z_value_code;
    GSVector_t xyz_point;           /* Vector of structures defining surface trace points */
    } GSSurfaceTrace_t;

typedef struct 
    {
    GSSurfaceSetName_t surface_set_name;    /* Name of intersecting surface set (horizon or fault) */
    GSVector_t surface_trace;       /* Vector of structures defining traces */
    } GSSurfaceSurfaceIntersection_t;

typedef struct                      /* new object pointed from GSSurfaceSet_t */
    {
    GSSurfaceSetName_t surface_set_name;    /* name of the surface set intersected */
    GSUnit_t depth_unit;            /* depth unit type for all depth representations */
    GSUnit_t time_unit;             /* time unit for all time representations */
    GSVector_t seismic_line_intersection;   /* Vector of section interpretation */
    GSVector_t section_intersection;    /* Vector of cross section intersection */
    GSVector_t well_marker;         /* Vector of GSWellMarker_t */
    GSVector_t seismic_well_tie;    /* Vector of GSSeismicWellTie_t */
    GSVector_t surface_surface_intersection;    /* vector of surface surface intersections */
    } GSSurfaceIntersection_t;

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
    GSPolylineType_t open_closed;   /* Open polyline or polygon ? */
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
        GS_JEFFERSONIAN = 1,
        GS_GULF_COAST_OFFSHORE = 2,
        GS_TEXAS_ABSTRACT = 3,
        GS_OCS_OFFSHORE = 4
        } type;
    union 
        {
        struct 
            {                       /* structure describing Jeffersonian description of
                                       a location */
            GSLongInt_t meridian_code;  /* PRINCIPAL_MERIDIAN code number - (Note: we
                                         need to publish a list or cite a reference - jgg) */
            GSReal_t township;      /* township number */
            GSCompassPoint_t twpns; /* 'N' or 'S' of survey base line */
            GSReal_t range;         /* range number */
            GSCompassPoint_t rngns; /* 'E' or 'W' of survey principal meridian */
            GSReal_t section;       /* section number in township */
            GSString_t section_type;    /* type of 'section' (NULL = section, 'LOT', etc */
            GSString_t spot;        /* Point in section from which measurements are references */
            GSString_t state;       /* State or province abbreviation */
            GSString_t country;     /* Country abbreviation */
            GSString_t county;      /* County, district, parish name */
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
                GSString_t alpha;   /* Optional character part of block 
                                       identifier */
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
            GSString_t utm_grid_block;  /* Universal Transverse Mercator block
                                           identifier */
            GSLongInt_t ocs_number;
            } ocs_offshore;         /* Outer Continental Shelf description */
        
        } system;
    } GSLegal_t;

/* the following structure describes the location of a point in a legal entity
as an offset from some point in the entity */

typedef struct 
    {
    GSLegal_t legal;                /* legal description of parcel */
    GSDistance_t nsfootage;         /* 'footage' in north-south direction */
    GSString_t ns;                  /* reference line of north-south 'footage'
                                       ('FEL','FWL') */
    GSDistance_t ewfootage;         /* 'footage' in ease-west direction */
    GSString_t ew;                  /* reference line of east-west 'footage'
                                       ('FNL','FSL') */
    } GSLegalLoc_t;

/* the following structure describes the geometry of a legal ownership boundary */

typedef struct 
    {
    GSLegal_t legal;                /* legal description of parcel */
    GSVector_t node_id;             /* list of nodes which identify corners of polygon.
                                       Use this if a topologic model is being used
                                       to transmit the perimeters.  The nodes refer
                                       to the map_location vector sent with the
                                       GSLandNetList_t structure.  If there are any holes
                                       then you must use the map_polyline vector. */
    GSVector_t map_polyline;        /* Vector of GSMapPolyline_t (NOTE: all polylines
                                       should have the GSPolylineType_t be
                                       GS_CLOSED_POLYGON).  Use this vector if
                                       explicit polygons need to be sent. */
    } GSLandNet_t;

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
    GSReal_t dx;                    /* X deviation from top-hole position in
                                       projected coordinate system (longitude if
                                       geodetic) */
    GSReal_t dy;                    /* Y deviation from top-hole position in
                                       projected coordinate system (latitude if
                                       geodetic) */
    } GSDepth_t;

typedef struct 
    {                               /* structure for lithostrat codes (mandatory) */
    GSString_t code;                /* Formation code of the lithostratigraphic unit */
    GSString_t age;                 /* Client-specified age code of
                                     lithostratigraphic unit (optional) */
    } GSLithoStrat_t;

/************************************************************************************/
/*                                                                                  */
/*                            WELL HEADER STRUCTURE                                 */
/*                                                                                  */
/************************************************************************************/

typedef struct 
    {                               /* structure defining well header information */
    GSUWI_t uwi;                    /* Unique Well Identifier (mandatory) */
    GSPlotSymbol_t symbol;          /* Plot Symbol name */
    
    GSUnit_t depth_unit;            /* Unit of measure for all depths in this well */
    GSUnit_t time_unit;             /* Unit of measurement for all times in this well */
    
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
    GSLegalLoc_t tophole;           /* structure describing legal status of the 
                                     tophole */
    GSMapLocation_t bottomhole_xy;  /* structure defining coordinates of 
                                    bottom hole position */
    GSLegalLoc_t bottomhole;        /* structure describing legal status of the  
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
    GSElevation_t ground_elevation; /* ground elevation */
    GSElevation_t kb_elevation;     /* kelly bushing elevation */
    GSElevation_t cf_elevation;     /* casing flange elevation */
    
    /* Checkshot information */
    struct 
        {
        GSElevationRef_t datum;     /* datum used for checkshot */
        GSVelocity_t vsd;           /* velocity surface to datum */
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
    GSVector_t stratcolumn;         /* Vector of strat column name to use for this well */
    enum 
        {
        GS_IMPERIAL_UNIT = 1,
        GS_METRIC_UNIT = 2
        } original_unit;            /* Original unit system that data was 
                                    collected in */
    GSBoolean_t proprietary;        /* TRUE if this well contains proprietary 
                                     information */
    GSBoolean_t discovery_well;     /* TRUE if this is the discovery field for the field*/
    GSBoolean_t fault;              /* TRUE if this well has a faulted section */
    
    /* Confidentiality Information */
    struct 
        {
        GSBoolean_t flag;           /* confidentiality flag */
        GSString_t formcode;        /* confidentiality formation */
        GSDepth_t depth;            /* confidentiality depth */
        GSDate_t release_date;      /* confidential release date */
        } confidential;
    
    /* Dates */
    GSDate_t last_update;           /* date/time of last update */
    GSDate_t spud_date;             /* spud date */
    GSDate_t comp_date;             /* completion date */
    GSDate_t rigrel;                /* rig release date */
    GSDate_t onprod;                /* on production date */
    GSDate_t calc_onprod;           /* calculated on production date */
    GSDate_t oninject;              /* on injection date */
    GSDate_t status_date;           /* date last status was entered */
    GSDate_t fin_drill;             /* final drilling date */
    
    /* Headers (tree-tops) for other well data classes */
    GSVector_t log_run;             /* vector of GSLogRun_t */
    GSVector_t well_core;           /* Vector of core data (GSWellCore_t) */
    GSVector_t zone_value;          /* Vector of zone values (GSZoneValue_t) */
    GSVector_t well_marker;         /* Vector of GSWellMarker_t */
    GSVector_t well_production;     /* Vector of GSWellProduction_t
                                     (sorted descending by year) */
    GSVector_t well_tubulars;       /* Vector of tubular products placed in well */
    GSVector_t well_remarks;        /* Vector of general remarks */
    GSVector_t well_test_hdr;       /* Vector of GSWellTestHdr_t */
    GSVector_t seismic_well_tie;    /* vector of correlations with seismic data */
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
        GS_WELL_CORE_NORMAL = 1,    /* Normal well core */
        GS_WELL_CORE_SIDEWALL = 2   /* Sidewall core */
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
structures.  The intent is to allow another vendor to display this information somehow
without being concerned about the semantics of the information.  This might be used
to display arbitrary information in a cross-section panel, or in a report. */

typedef struct 
    {
    GSString_t type_name;           /* Type of data being described, or name of data set
                                       (see zone variable list for definition) */
    GSDepth_t depth;                /* Depth at this these observations were made
                                      (Zero implies these observations apply
                                      to the well as a whole) */
    GSVector_t string;              /* A vector of strings describing some event
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
    GSUnit_t z_unit;                /* Unit of depth (time) measurement */
    GSUnit_t trace_unit;            /* Unit of trace value */
    GSReal_t non_null_top;          /* Top of trace data interval (excluding nulls) */
    GSReal_t non_null_base;         /* Base of trace data interval (excluding nulls) */
    GSReal_t min;                   /* Minimum trace value */
    GSReal_t max;                   /* Maximum trace value */
    GSReal_t dinc;                  /* Digitizing increment (0.0 if random) */
    GSVector_t processing_history;  /* Vector of processing history records sorted by date,
                                     most recent listed first */
    GSVector_t depth;               /* Array of depth values (4 or 8 byte floating point values)
                                     Note: this is NOT a GSDepths_t structure) */
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
    It usually is associated with a single billing cycle
    Also called an 'activity' */
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
        GSMudResistivity_t resistivity; /* Resistivity of mud */
        GSMudResistivity_t resistivity_bht; /* Resistivity of mud at bottom hole
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
    GSBoolean_t numeric;            /* TRUE if measurements are often numeric */
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
    GSMeasurement_t value;          /* Value of zone variable as a number */
    GSString_t string_value;        /* (optional) value of zone variable as a string
                                       (for such things as 'indurated', 'friable',
                                       'overpressured', etc) */
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
        GS_OIL = 1,
        GS_WATER = 2,
        GS_GAS = 3
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
    GS_LITHO_TOP = 1,               /* Lithostratigraphic (formation) top */
    GS_CONTACT_TOP = 2,             /* Fluid surface contact (e.g. oil/water) */
    GS_FAULT_POINT = 3,             /* Piercing point of well through fault plane */
    GS_MARKER_TOP = 4,              /* Stratigraphic marker - subordinate to
                                       lithostratigraphic pick */
    GS_BIOSTRAT = 5,                /* Biostratigraphic boundary */
    GS_OTHER_TOP = 6
    } GSMarkerType_t;

typedef struct 
    {                               /* Formation top description */
    GSUWI_t uwi;                    /* Unique well identifier (mandatory) */
    GSLithoStrat_t name;            /* Name of lithostratigraphic (surface set) name (mandatory) */
    GSString_t source;              /* Source of interpretation */
    GSSurfaceSetName_t seismic_horizon; /* Name of seismic surface that this marker is
                                           correlated with */
    GSUnit_t z_unit;                /* unit of measure (used when part of a surface set) */
    GSDepth_t depth;                /* Coordinates of piercing point */
    GSMeasurement_t fault_gap;      /* For faults, the amount of missing section faulted
                                       out at this location */
    enum 
        {
        GS_NORMAL_SECTION = 0,      /* Paleo up indicators point up */
        GS_INVERTED_SECTION = 1     /* Paleo up indicators inverted */
        } up_direction;
    GSString_t comment;             /* comments describing nature of pick
                                     (ABSENT, ERODED, NOT PICKED, etc) */
    GSBoolean_t best_pick;          /* TRUE if this is best available information */
    GSString_t quality;             /* Quality code of pick */
    GSDate_t last_update;           /* date this interpretation made */
    } GSWellMarker_t;

/*    Well Tie Information  
      --------------------
 */

typedef struct                      /* This structure describes the relationship between
                                       a specific surface in a well and its correlation
                                       with a specify seismic line and shot point.
                                       Useful for tying geophysical/geological data in
                                       reservoir simulators */
    {
    GSLithoStrat_t marker;
    GSSurfaceSetName_t surface_set_name;    /* Name of correlative seismic surface */
    GSSeismicSurveyName_t survey_name;  /* (optional) seismic survey name */
    GSSeismicLineName_t line_name;  /* (optional) seismic line name */
    GSSeismicDataPoint_t seismic_loc;   /* location on seismic line */
    } GSSeismicWellTie_t;

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
/*                           WELL TEST STRUCTURES                                   */
/*                                                                                  */
/************************************************************************************/

typedef struct 
    {
    enum 
        {
        GS_WELL_DRILLSTEM_TEST,
        GS_WELL_PRODUCTION_TEST
        } test_type;
    /* this needs mucho work */
    } GSWellTestHdr_t;

/************************************************************************************/
/*                                                                                  */
/*                                  CROSS SECTIONS                                  */
/*                                                                                  */
/************************************************************************************/

/*  Geological Cross Section is a structure designed for describing a
    cross section with surfaces, zones, and wells projected.  This can
    be either one traverse section or segmented section consisting of
    more than one section panel.

    The vector of wells should contain the list of wells which may be
    presented with this section by projecting their data to the section.

    GSSectionIntersection_t is an intersection between one surface set
    with the cross section, the points on which are described by the
    pair of x and z values.  X values are measured as a distance from
    the start point of the section, that is, it is the X coordinate
    in the cross section display.
    
    The zones above and below each point on the section can be specified
    using up_zone_number and down_zone_number vector together with
    the vector of zone name which appears in GSGeolCrossSection_t. 
    
    These structures are also inherited by the seismic objects.  The local coordinate
    system that has to be imposed for seismic sections is done in the
    GSSeismicLineIntersection_t structure. */

/*    Generic Cross Section
      ---------------------
*/

typedef struct 
    {
    GSString_t section_name;        /* Name of the section if applicable */
    GSVector_t map_location;        /* Map location of the nodes of the
                                       traverse segmented section.  A vector
                                       of two values (start and stop point)
                                       in case of a single traverse section */
    GSVector_t section_intersection;    /* Vector of intersection with one surface
                                           set */
    GSVector_t zone_name;           /* Sorted vector of zone names found on section;
                                       1st entry is shallowest */
    } GSSection_t;

/*    Geological Cross Section
      ------------------------
*/

typedef struct 
    {
    GSSection_t section;            /* Geometric description of the section */
    GSVector_t uwi;                 /* Vector of wells to be projected onto
                                       this cross section */
    GSVector_t proj_line;           /* Section panel number on which each well 
                                       is to be projected (section panel no.
                                       as 1, 2, ..., ; 4 byte integer).
                                       0 means the closest section panel */
    } GSGeolCrossSection_t;

/************************************************************************************/
/*                                                                                  */
/*                    TOP STRUCTURES FOR EACH DATA TYPE                             */
/*                                                                                  */
/************************************************************************************/

/*    Seismic Survey Information
      --------------------------
 */
/*
All seismic related information is specified in a hierarchical structure.  The
top of the hierarchy is a structure which groups the information according to
surveys.  Surveys may be either 2d groups of line or 3d surveys.  In order to
facilitate memory management, 3d survey data will be organized according to
lines.  The following defines the survey structure:                  */

typedef struct 
    {
    GSSeismicSurveyName_t survey_name;  /* Name of survey */
    GSString_t survey_desc;         /* Survey description */
    enum                            /* Survey type */
        {
        GS_SURVEY_2D = 2,           /* 2d survey symbol */
        GS_SURVEY_3D = 3            /* 3d survey symbol */
        } survey_type;
    GSElevationRef_t z_datum;       /* Datum to use in association with elevations */
    GSCartoProjection_t proj;       /* Projection information */
    GSNullValue_t null_value;       /* null values for various data types */
    GSSeismicLocation3D_t loc3d;    /* 3d survey specification and
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
    GSNullValue_t null_value;       /* null values for various data types */
    GSSurfaceType_t surface_type;   /* Type of surface (Fault, horizon, geologic marker) */
    GSVector_t surface_geometry;    /* List of geometry information for surface */
    GSSurfaceIntersection_t intersection;   /* Intersection of surface set with other entities */
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
    GSNullValue_t null_value;       /* null values for various data types */
    GSVector_t well_hdr;            /* vector of structures describing wells */
    GSVector_t geol_cross_section;  /* Vector of geological cross section */
    } GSField_t;

/*    Map Information
      ---------------
 */

typedef struct 
    {
    GSString_t source_name;         /* Name of the data source. A null
                                       value is allowed (e.g. no source) */
    GSCartoProjection_t proj;       /* Projection information */
    GSNullValue_t null_value;       /* null values for various data types */
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
    GSNullValue_t null_value;       /* null values for various data types */
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
    GSNullValue_t null_value;       /* null values for various data types */
    } GSLithostratCodeList_t;

/*    List of zone definitions and variables
      --------------------------------------
 */
typedef struct 
    {
    GSVector_t zone_definition;     /* List of zone definition */
    GSVector_t zone_variable_definition;    /* List of zone variable definitions */
    GSNullValue_t null_value;       /* null values for various data types */
    } GSZoneList_t;

/*    Land Net Information
      ---------------
 */

typedef struct 
    {
    GSString_t source_name;         /* Name of the data source. A null
                                       value is allowed (e.g. no source) */
    GSCartoProjection_t proj;       /* Projection information */
    GSNullValue_t null_value;       /* null values for various data types */
    GSExtents_t extent;             /* Area of interest of the land net list */
    enum 
        {
        GS_POLYLINE_MODEL = 1,
        GS_TOPOLOGIC_MODEL = 2
        } model_type;               /* flag indicating whether a topologic
                                       or a polyline model is being used to
                                       represent the polygons of the land net */
    GSVector_t map_location;        /* Array of node coordinates.  When used,
                                       the land net polygons are defined as a
                                       list of nodes which refer to this list.
                                       If a topologic model is not used, this
                                       vector should be left null. */
    GSVector_t land_net;
    } GSLandNetList_t;

/************************************************************************************/
/*                                                                                  */
/*                    STATUS STRUCTURE                                              */
/*                                                                                  */
/************************************************************************************/

/*  A status structure for returning to the exporting application information
    about the success/failure of the various functions performed by the application server.
 */

typedef struct 
    {
    GSLongInt_t status_code;        /* error code */
    GSString_t object_key;          /* One or more object keys, separated by ^,
                                       identifying entity that produced status */
    GSString_t status_description;  /* Status description */
    } GSStatusReturn_t;

typedef GSVector_t GSStatusVector_t;

/************************************************************************************/
/*                                                                                  */
/*                    CONTEXT STRUCTURES                                            */
/*                                                                                  */
/************************************************************************************/

/*  There are two parts to application server context within which a client must operate,
    environmental in the form of a possible memory limitation, and data in the
    form of things such as project names, pass words, file names, etc.  The
    following structures will be used to reply to a request for context
    information : */

typedef struct 
    {
    GSLongInt_t max_memory;         /* Size of local memory buffer, in bytes */
    GSVector_t context_requirements;    /* data context requirements */
    } GSAppServerContext_t;

/*  This structure is associated with defining the information 
to be sent to to the application server to establish context. */

typedef struct 
    {
    enum 
        {
        GS_INTEGER_TYPE = 1,
        GS_CHARACTER_TYPE = 2,
        GS_FLOAT_TYPE = 3,
        GS_UNSIGNED_INTEGER = 4
        } type;                     /* Data type of context variable */
    GSLongInt_t max_length;         /* Maximum length of context variable (if string) */
    GSBoolean_t confidential;       /* Flag for whether to display user's input */
    GSString_t prompt;              /* Prompt string to use for requesting context variable */
    GSString_t cmdline_tag;             
    } GSContextRequirements_t;

/*  This vector is used to transmit the requested information back to
the exporting process.  There will be one string for each requirement specified
in the previous structure.  Even though an integer(perhaps unsigned) or float may be
required, it will be sent in ASCII representation. */

typedef GSVector_t GSContextInformation_t;  /* this is a vector of type GSString_t */

/*   Upon establishment of the context, a token will be returned to the client
     process */

typedef GSLongInt_t GSContextToken_t;   /*  tag for context established */

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

typedef enum 
    {
    /*    Instruction is unknown, check disposition_ext */
    GS_CMD_UNKNOWN = 0,             /* Address of structure is undefined */
    /*    Insert into the appropriate place(s) the contents of the structure
          (Reject if keys already exist) */
    GS_CMD_INSERT = 1,
    /*    Update the appropriate place(s) with the contents of the structure
          (Insert or update if necessary based on keys) */
    GS_CMD_UPDATE = 2,
    /*    Overwrite the appropriate place(s) using the keys of the structure
          (but do not insert if keys do not exist) */
    GS_CMD_OVERWRITE = 3,
    /*    Delete the appropriate place(s) using the keys of the structure */
    GS_CMD_REPLACE = 4,
    /*    Delete the appropriate place(s) using the keys of the structure
          if it exists, then perform insert) */
    GS_CMD_DELETE = 5,
    /*    Interaction is complete, terminate                */
    GS_STOP_SERVER = 1000,          /* Address of structure is NULL */
    /*    Break current context, retain comm handshake      */
    GS_CONTEXT_BREAK = 1001,        /* Address of structure is NULL */
    /*    Return context requirements                       */
    GS_SEND_CONTEXT = 1002,         /* Address of structure is NULL */
    /*    Process everything, send no return status         */
    GS_NO_RETURN = 1003,            /* Address of structure is NULL */
    /*    Command from GeoShare Server to open another port */
    GS_OPEN_PORT = 1004             /* Address of structure is NULL */
    } GSOperation_t;

/*  The following is a list of top-level structures ('objects') that GeoShare defines.
    The list is extendable past the enum set by using the extension portion of the union. */

typedef enum 
    {
    GS_STRUCT_UNKNOWN = 0,          /* structure unknown to GeoShare is passed,
                                       check type_ext for value */
    GS_STRUCT_APP_SERVER_SPECS = 1,
    GS_STRUCT_APP_SERVER_RQMTS = 2,
    GS_STRUCT_CONTEXT_RQMTS = 3,
    GS_STRUCT_CONTEXT_INFO = 4,
    GS_STRUCT_CONTEXT_TOKEN = 5,
    GS_STRUCT_STATUS_VECTOR = 6,
    GS_STRUCT_SEISMIC_SURVEY = 11,
    GS_STRUCT_SURFACE_SET = 12,
    GS_STRUCT_FIELD = 13,
    GS_STRUCT_MAP = 14,
    GS_STRUCT_FAULT_TRACE_SET = 15,
    GS_STRUCT_LITHOSTRAT_CODE_LIST = 16,
    GS_STRUCT_ZONE_LIST = 17,
    GS_STRUCT_LAND_NET_LIST = 18
    } GSDataStructType_t;

typedef struct 
    {
    GSLongInt_t geoscience;         /* geoscience version number */
    GSLongInt_t revision;           /* Revision number of structure */
    GSString_t vendor;              /* Vendor that provided latest revision */
    } GSVersion_t;

typedef struct 
    {
    GSOperation_t operator;
    GSLongInt_t operation_ext;
    GSDataStructType_t type;
    GSLongInt_t type_ext;
    GSVersion_t version;
    GSVoidptr_t structure_pointer;
    } GSStructure_t;

typedef struct 
    {
    GSContextToken_t context_token; /* tag for context established */
    GSVector_t structure;           /* vector of structures on which to act */
    } GSExecutive_t;

typedef GSLongInt_t GSConnection_t; /* communications channel identifier */

/************************************************************************************/
/*                                                                                  */
/*                    APPLICATION SERVER CAPABILITY STRUCTURES                      */
/*                                                                                  */
/************************************************************************************/

/*  A application server information structure is returned to the GeoShare Server
    upon request.  This structure contains the following information:
        a.  Status of application server service initialization
        b.  A unique name for the application server so a client can ask for it by name (if known).
        c.  A descriptive text string to show the user in a list of application servers
        d.  Exploration data structures supported
        e.  Data structure revision level range supported
 */

/*  Support for a given data structure is composed of three parts, an enumerated
    parameter which indicates that it is supported, a mask variable in which
    the application server indicates which components of the structure it can handle, and 
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
#define GSM_WELL_DSTS          (1<<9)   /* Drillstem tests are supported */
#define GSM_WELL_PROD_TESTS    (1<<10)  /* Production tests are supported */
#define GSM_SEISMIC_WELL_TIE   (1<<11)  /* Seismic well tie data are supported */

/*  Field information Component Masks */

#define GSM_FIELD_WELLS        (1<<0)   /* Field Well data are supported */
#define GSM_FIELD_BOUNDARY     (1<<1)   /* Field boundary is supported */
#define GSM_FIELD_ZONE_VALUE   (1<<2)   /* Field zone data are supported */

/*  Land Net Component Masks */

#define GSM_LAND_NET_JEFFERSONIAN (1<<0)   /* Jeffersonian data are supported */
#define GSM_LAND_NET_TEXAS_ABSTRACT (1<<1)   /* Texas abstracts are supported */
#define GSM_LAND_NET_ATS (1<<2)   /* Alberta Township Survey data are supported (Canada) */
        
#define GSM_LAND_NET_DLS (1<<3)   /* Dominion Land Survey data are supported (Canada) */

#define GSM_LAND_NET_NTS (1<<4)   /* NTS data are supported (Canada) */
#define GSM_LAND_NET_OCS (1<<5)   /* US Offshore data are supported */
#define GSM_LAND_NET_GULF_COAST (1<<6)   /* US Gulf shore data are supported */

/*  Server Application must provide one of these for each data structure it
    supports - also the type of operations supported */

typedef struct 
    {
    GSDataStructType_t data_struct_type;
    GSMask_t components_supported;
    GSVersion_t version;
    GSVector_t operation;           /* supported operations on these data
                                       (e.g. insert, update, overwrite, replace) */
    } GSDataSupported_t;

/*  This structure contains the information which will be given to the GeoShare
    server each time it starts an instance of a server application  */

typedef struct 
    {
    GSLongInt_t app_server_status;  /* Reserved for future use */
    GSString_t app_server_name;     /* Unique character identifier of application server
                                     (<=20 characters ) */
    GSString_t banner_string;       /* Your chance to advertise your product!
                                       (<=60 characters ) */
    GSVector_t data_supported;      /* List of types of data the server supports */
    } GSAppServerSpecs_t;

/*  A client application may request information from the GeoShare server about
    which application servers are available for the task the client wishes.  It
    may specify the request in terms of a application server name or in terms of data
    structures supported.  If more than one specification is provided, then
    application servers meeting any of the specifications will be included.  A vector of 
    the following structures will be passed to the GeoShare server as input to 
    making a list of application servers which will satisfy the criteria: */

typedef GSVector_t GSAppServer_t;

typedef struct 
    {
    enum 
        {
        GS_STRUCT_SUPPORTED = 1,
        GS_NAME = 2
        } spec_type;
    GSString_t app_server_name;
    GSDataSupported_t data;
    } GSRequirement_t;

/************************************************************************************/
/*                                                                                  */
/*                    END OF INCLUDE FILE GEOSCIENCE.H                              */
/*                                                                                  */
/************************************************************************************/
#endif
