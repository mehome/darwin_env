/*
 * Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2005 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart, 
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2006      University of Houston. All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */
#include "ompi_config.h"
#include <stdio.h>

#include "ompi/mpi/c/bindings.h"
#include "ompi/errhandler/errcode.h"
#include "ompi/attribute/attribute.h"

#if OMPI_HAVE_WEAK_SYMBOLS && OMPI_PROFILING_DEFINES
#pragma weak MPI_Add_error_code = PMPI_Add_error_code
#endif

#if OMPI_PROFILING_DEFINES
#include "ompi/mpi/c/profile/defines.h"
#endif

static const char FUNC_NAME[] = "MPI_Add_error_code";


int MPI_Add_error_code(int errorclass, int *errorcode)
{
    int code;
    int rc;

    if ( MPI_PARAM_CHECK ) {
        OMPI_ERR_INIT_FINALIZE(FUNC_NAME);

        if ( ompi_mpi_errcode_is_invalid(errorclass) )
            return OMPI_ERRHANDLER_INVOKE(MPI_COMM_WORLD, MPI_ERR_ARG,
                                          FUNC_NAME);
        
	if ( !ompi_mpi_errnum_is_class ( errorclass) ) 
	    return OMPI_ERRHANDLER_INVOKE(MPI_COMM_WORLD, MPI_ERR_ARG,
					  FUNC_NAME);
    }
    
    code = ompi_mpi_errcode_add ( errorclass);
    if ( 0 > code ) {
        return OMPI_ERRHANDLER_INVOKE(MPI_COMM_WORLD, MPI_ERR_INTERN,
                                      FUNC_NAME);
    }

    /* 
    ** Update the attribute value. See the comments
    ** in attribute/attribute.c and attribute/attribute_predefined.c
    ** why we have to call the fortran attr_set function 
    */
    rc  = ompi_attr_set_fortran_mpi1 (COMM_ATTR, 
				      MPI_COMM_WORLD,
				      &MPI_COMM_WORLD->c_keyhash,
				      MPI_LASTUSEDCODE, 
				      ompi_mpi_errcode_lastused,
				      true, true);
    if ( MPI_SUCCESS != rc ) {
	return OMPI_ERRHANDLER_INVOKE(MPI_COMM_WORLD, rc, FUNC_NAME);
    }
    
    *errorcode = code;
    return MPI_SUCCESS;
}