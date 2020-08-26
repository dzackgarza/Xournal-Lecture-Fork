/*
 * Copyright � 2002, 2003 Sun Microsystems, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Sun Microsystems, Inc. nor the names of 
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * This software is provided "AS IS," without a warranty of any kind.
 *
 * ALL EXPRESS OR IMPLIED CONDITIONS, REPRESENTATIONS AND WARRANTIES,
 * INCLUDING ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE OR NON-INFRINGEMENT, ARE HEREBY EXCLUDED.
 * SUN AND ITS LICENSORS SHALL NOT BE LIABLE FOR ANY DAMAGES OR
 * LIABILITIES SUFFERED BY LICENSEE AS A RESULT OF OR RELATING TO USE,
 * MODIFICATION OR DISTRIBUTION OF THE SOFTWARE OR ITS DERIVATIVES.
 * IN NO EVENT WILL SUN OR ITS LICENSORS BE LIABLE FOR ANY LOST REVENUE,
 * PROFIT OR DATA, OR FOR DIRECT, INDIRECT, SPECIAL, CONSEQUENTIAL,
 * INCIDENTAL OR PUNITIVE DAMAGES, HOWEVER CAUSED AND REGARDLESS OF THE
 * THEORY OF LIABILITY, ARISING OUT OF THE USE OF OR INABILITY TO USE
 * SOFTWARE, EVEN IF SUN HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 *
 */

/* $Id$ */
/* @(#)ttcr.h 1.6 03/01/08 SMI */

/*
 * @file ttcr.h
 * @brief TrueType font creator
 * @author Alexander Gelfenbain <adg@sun.com>
 * @version 1.1
 *
 */


#ifndef __TTCR_H
#define __TTCR_H

#include "sft.h"
#include "list.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef struct _TrueTypeCreator TrueTypeCreator;

/* TrueType data types */
typedef struct {
    guint16 aw;
    gint16  lsb;
} longHorMetrics;

/* A generic base class for all TrueType tables */
typedef struct {
    guint32 tag;                         /* table tag                                                */
    guint8   *rawdata;                    /* raw data allocated by GetRawData_*()                     */
    void   *data;                       /* table specific data                                      */
} TrueTypeTable;

/** Error codes for most functions */
enum TTCRErrCodes {                     
    TTCR_OK = 0,                        /**< no error                                               */
    TTCR_ZEROGLYPHS = 1,                /**< At least one glyph should be defined                   */
    TTCR_UNKNOWN = 2,                   /**< Unknown TrueType table                                 */
    TTCR_GLYPHSEQ = 3,                  /**< Glyph IDs are not sequential in the glyf table         */
    TTCR_NONAMES = 4,                   /**< 'name' table does not contain any names                */
    TTCR_NAMETOOLONG = 5,               /**< 'name' table is too long (string data > 64K)           */
    TTCR_POSTFORMAT = 6                 /**< unsupported format of a 'post' table                   */
};

/* ============================================================================
 *
 * TrueTypeCreator methods
 *
 * ============================================================================ */

/*
 * TrueTypeCreator constructor.
 * Allocates all internal structures.
 */
void TrueTypeCreatorNewEmpty(guint32 tag, TrueTypeCreator **_this);

/*
 * TrueTypeCreator destructor. It calls destructors for all TrueTypeTables added to it.
 */
void TrueTypeCreatorDispose(TrueTypeCreator *_this);

/*
 * Adds a TrueType table to the TrueType creator.
 * SF_TABLEFORMAT value.
 * @return value of SFErrCodes type
 */
int AddTable(TrueTypeCreator *_this, TrueTypeTable *table);

/*
 * Removes a TrueType table from the TrueType creator if it is stored there.
 * It also calls a TrueTypeTable destructor.
 * Note: all generic tables (with tag 0) will be removed if this function is
 * called with the second argument of 0.
 * @return value of SFErrCodes type
 */
void RemoveTable(TrueTypeCreator *_this, guint32 tag);



/*
 * Writes a TrueType font generated by the TrueTypeCreator to a segment of
 * memory that this method allocates. When it is not needed anymore the caller
 * is supposed to call free() on it.
 * @return value of SFErrCodes type
 */
int StreamToMemory(TrueTypeCreator *_this, guint8 **ptr, guint32 *length);

/*
 * Writes a TrueType font  generated by the TrueTypeCreator to a file
 * @return value of SFErrCodes type
 */
int StreamToFile(TrueTypeCreator *_this, const char* fname);


/* ============================================================================
 *
 * TrueTypeTable methods
 *
 * ============================================================================ */

/*
 * Destructor for the TrueTypeTable object.
 */
void TrueTypeTableDispose(TrueTypeTable *);

/*
 * This function converts the data of a TrueType table to a raw array of bytes.
 * It may allocates the memory for it and returns the size of the raw data in bytes.
 * If memory is allocated it does not need to be freed by the caller of this function,
 * since the pointer to it is stored in the TrueTypeTable and it is freed by the destructor
 * @return TTCRErrCode
 *
 */

int GetRawData(TrueTypeTable *, guint8 **ptr, guint32 *len, guint32 *tag);

/*
 *
 * Creates a new raw TrueType table. The difference between this constructor and
 * TrueTypeTableNew_tag constructors is that the latter create structured tables
 * while this constructor just copies memory pointed to by ptr to its buffer
 * and stores its length. This constructor is suitable for data that is not
 * supposed to be processed in any way, just written to the resulting TTF file.
 */
TrueTypeTable *TrueTypeTableNew(guint32 tag,
                                guint32 nbytes,
                                guint8 *ptr);

/*
 * Creates a new 'head' table for a TrueType font.
 * Allocates memory for it. Since a lot of values in the 'head' table depend on the
 * rest of the tables in the TrueType font this table should be the last one added
 * to the font.
 */
TrueTypeTable *TrueTypeTableNew_head(guint32 fontRevision,
                                     guint16 flags,
                                     guint16 unitsPerEm,
                                     guint8  *created,
                                     guint16 macStyle,
                                     guint16 lowestRecPPEM,
                                     gint16  fontDirectionHint);

/*
 * Creates a new 'hhea' table for a TrueType font.
 * Allocates memory for it and stores it in the hhea pointer.
 */
TrueTypeTable *TrueTypeTableNew_hhea(gint16  ascender,
                                     gint16  descender,
                                     gint16  linegap,
                                     gint16  caretSlopeRise,
                                     gint16  caretSlopeRun);

/*
 * Creates a new empty 'loca' table for a TrueType font.
 *
 * INTERNAL: gets called only from ProcessTables();
 */
TrueTypeTable *TrueTypeTableNew_loca(void);

/*
 * Creates a new 'maxp' table based on an existing maxp table.
 * If maxp is 0, a new empty maxp table is created
 * size specifies the size of existing maxp table for
 * error-checking purposes
 */
TrueTypeTable *TrueTypeTableNew_maxp(guint8 *maxp, int size);

/*
 * Creates a new empty 'glyf' table.
 */
TrueTypeTable *TrueTypeTableNew_glyf(void);

/*
 * Creates a new empty 'cmap' table.
 */
TrueTypeTable *TrueTypeTableNew_cmap(void);

/*
 * Creates a new 'name' table. If n != 0 the table gets populated by
 * the Name Records stored in the nr array. This function allocates
 * memory for its own copy of NameRecords, so nr array has to
 * be explicitly deallocated when it is not needed.
 */
TrueTypeTable *TrueTypeTableNew_name(int n, NameRecord *nr);

/*
 * Creates a new 'post' table of one of the supported formats
 */
TrueTypeTable *TrueTypeTableNew_post(guint32 format,
                                     guint32 italicAngle,
                                     gint16 underlinePosition,
                                     gint16 underlineThickness,
                                     guint32 isFixedPitch);


/*------------------------------------------------------------------------------
 *
 *  Table manipulation functions
 *
 *------------------------------------------------------------------------------*/
 

/*
 * Add a character/glyph pair to a cmap table
 */
void cmapAdd(TrueTypeTable *, guint32 id, guint32 c, guint32 g);

/*
 * Add a glyph to a glyf table.
 *
 * @return glyphID of the glyph in the new font
 *
 * NOTE: This function does not duplicate GlyphData, so memory will be
 * deallocated in the table destructor
 */
guint32 glyfAdd(TrueTypeTable *, GlyphData *glyphdata, TrueTypeFont *fnt);

/*
 * Query the number of glyphs currently stored in the 'glyf' table
 *
 */
guint32 glyfCount(const TrueTypeTable *);

/*
 * Add a Name Record to a name table.
 * NOTE: This function duplicates NameRecord, so the argument
 * has to be deallocated by the caller (unlike glyfAdd)
 */
void nameAdd(TrueTypeTable *, NameRecord *nr);

                   

/*
 * Private Data Types
 */

struct _TrueTypeCreator {
    guint32 tag;                         /**< TrueType file tag */
    list   tables;                      /**< List of table tags and pointers */
};



#ifdef __cplusplus
}
#endif

#endif /* __TTCR_H */
