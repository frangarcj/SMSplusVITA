/* ioapi.c -- IO base function header for compress/uncompress .zip
   files using zlib + zip or unzip API

   Version 1.00, September 10th, 2003

   Copyright (C) 1998-2003 Gilles Vollant
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "zlib.h"
#include "ioapi.h"

#include <psp2/types.h>
#include <psp2/io/fcntl.h>


/* I've found an old Unix (a SunOS 4.1.3_U1) without all SEEK_* defined.... */

#ifndef SEEK_CUR
#define SEEK_CUR    1
#endif

#ifndef SEEK_END
#define SEEK_END    2
#endif

#ifndef SEEK_SET
#define SEEK_SET    0
#endif

voidpf ZCALLBACK fopen_file_func OF((
   voidpf opaque,
   const char* filename,
   int mode));

uLong ZCALLBACK fread_file_func OF((
   voidpf opaque,
   voidpf stream,
   void* buf,
   uLong size));

uLong ZCALLBACK fwrite_file_func OF((
   voidpf opaque,
   voidpf stream,
   const void* buf,
   uLong size));

long ZCALLBACK ftell_file_func OF((
   voidpf opaque,
   voidpf stream));

long ZCALLBACK fseek_file_func OF((
   voidpf opaque,
   voidpf stream,
   uLong offset,
   int origin));

int ZCALLBACK fclose_file_func OF((
   voidpf opaque,
   voidpf stream));

int ZCALLBACK ferror_file_func OF((
   voidpf opaque,
   voidpf stream));


voidpf ZCALLBACK fopen_file_func (opaque, filename, mode)
   voidpf opaque;
   const char* filename;
   int mode;
{
    SceUID file = NULL;
    int mode_fopen = 0;
    if ((mode & ZLIB_FILEFUNC_MODE_READWRITEFILTER)==ZLIB_FILEFUNC_MODE_READ)
        mode_fopen = PSP2_O_RDONLY;
    else
    if (mode & ZLIB_FILEFUNC_MODE_EXISTING)
        mode_fopen = PSP2_O_RDWR;
    else
    if (mode & ZLIB_FILEFUNC_MODE_CREATE)
        mode_fopen = PSP2_O_WRONLY | PSP2_O_CREAT;

    if ((filename!=NULL) && (mode_fopen != 0))
        file = sceIoOpen(filename, mode_fopen, 0777);
    return file;
}


uLong ZCALLBACK fread_file_func (opaque, stream, buf, size)
   voidpf opaque;
   voidpf stream;
   void* buf;
   uLong size;
{
    uLong ret;
    ret = sceIoRead((SceUID)stream, buf, (size_t)size);
    return ret;
}


uLong ZCALLBACK fwrite_file_func (opaque, stream, buf, size)
   voidpf opaque;
   voidpf stream;
   const void* buf;
   uLong size;
{
    uLong ret;
    ret = sceIoRead((SceUID)stream, buf, (size_t)size);
    return ret;
}

long ZCALLBACK ftell_file_func (opaque, stream)
   voidpf opaque;
   voidpf stream;
{
    long ret;
    ret = sceIoLseek((SceUID)stream,0,PSP2_SEEK_END);
    sceIoLseek((SceUID)stream,0,PSP2_SEEK_SET);
    return ret;
}

long ZCALLBACK fseek_file_func (opaque, stream, offset, origin)
   voidpf opaque;
   voidpf stream;
   uLong offset;
   int origin;
{
    int fseek_origin=0;
    long ret;
    switch (origin)
    {
    case ZLIB_FILEFUNC_SEEK_CUR :
        fseek_origin = PSP2_SEEK_CUR;
        break;
    case ZLIB_FILEFUNC_SEEK_END :
        fseek_origin = PSP2_SEEK_END;
        break;
    case ZLIB_FILEFUNC_SEEK_SET :
        fseek_origin = PSP2_SEEK_SET;
        break;
    default: return -1;
    }
    ret = 0;
    sceIoLseek((SceUID)stream,offset,fseek_origin);
    return ret;
}

int ZCALLBACK fclose_file_func (opaque, stream)
   voidpf opaque;
   voidpf stream;
{
    int ret;
    ret = sceIoClose((SceUID)stream);
    return ret;
}

int ZCALLBACK ferror_file_func (opaque, stream)
   voidpf opaque;
   voidpf stream;
{
    int ret = 0;
    //ret = ferror((FILE *)stream);
    return ret;
}

void fill_fopen_filefunc (pzlib_filefunc_def)
  zlib_filefunc_def* pzlib_filefunc_def;
{
    pzlib_filefunc_def->zopen_file = fopen_file_func;
    pzlib_filefunc_def->zread_file = fread_file_func;
    pzlib_filefunc_def->zwrite_file = fwrite_file_func;
    pzlib_filefunc_def->ztell_file = ftell_file_func;
    pzlib_filefunc_def->zseek_file = fseek_file_func;
    pzlib_filefunc_def->zclose_file = fclose_file_func;
    pzlib_filefunc_def->zerror_file = ferror_file_func;
    pzlib_filefunc_def->opaque = NULL;
}
