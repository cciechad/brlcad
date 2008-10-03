/*                     E N V . C
 * BRL-CAD / ADRT
 *
 * Copyright (c) 2002-2008 United States Government as represented by
 * the U.S. Army Research Laboratory.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this file; see the file named COPYING for more
 * information.
 */
/** @file env.c
 *
 *  Common Library - Environment Settings Parsing
 *
 */

#include "env.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bio.h"
#include "render.h"
#include "bu.h"


#if 0
void common_env_init(common_env_t *env);
void common_env_free(common_env_t *env);
void common_env_prep(common_env_t *env);
void common_env_read(common_env_t *env);
#endif

void common_env_init(common_env_t *env) {
    render_flat_init(&env->render);
    env->img_w = 640;
    env->img_h = 480;
    env->img_hs = 1;
    env->tile_w = 40;
    env->tile_h = 40;

    env->geometry_file[0] = 0;
    env->kdtree_cache_file[0] = 0;
    env->properties_file[0] = 0;
    env->textures_file[0] = 0;
    env->mesh_map_file[0] = 0;
    env->frames_file[0] = 0;
}


void common_env_free(common_env_t *env) {
}


void common_env_prep(common_env_t *env) {
    env->img_vw = env->img_w * env->img_hs;
    env->img_vh = env->img_h * env->img_hs;
}


void common_env_read(common_env_t *env, const char *fpath) {
    FILE *fh;
    char line[256], *token;

    fh = fopen(fpath, "r");
    if (!fh) {
	printf("Project file: %s does not exist, exiting...\n", fpath);
	exit(1);
    }

    while (bu_fgets(line, 256, fh)) {
	token = strtok(line, ",");

	if (!strcmp("geometry_file", token)) {
	    strncpy(env->geometry_file, strtok(NULL, ","), ADRT_NAME_SIZE);
	    env->geometry_file[strlen(env->geometry_file) - 1] = 0;
	} else if (!strcmp("kdtree_cache_file", token)) {
	    strncpy(env->kdtree_cache_file, strtok(NULL, ","), ADRT_NAME_SIZE);
	    env->kdtree_cache_file[strlen(env->kdtree_cache_file) - 1] = 0;
	} else if (!strcmp("properties_file", token)) {
	    strncpy(env->properties_file, strtok(NULL, ","), ADRT_NAME_SIZE);
	    env->properties_file[strlen(env->properties_file) - 1] = 0;
	} else if (!strcmp("textures_file", token)) {
	    strncpy(env->textures_file, strtok(NULL, ","), ADRT_NAME_SIZE);
	    env->textures_file[strlen(env->textures_file) - 1] = 0;
	} else if (!strcmp("mesh_map_file", token)) {
	    strncpy(env->mesh_map_file, strtok(NULL, ","), ADRT_NAME_SIZE);
	    env->mesh_map_file[strlen(env->mesh_map_file) - 1] = 0;
	} else if (!strcmp("frames_file", token)) {
	    strncpy(env->frames_file, strtok(NULL, ","), ADRT_NAME_SIZE);
	    env->frames_file[strlen(env->frames_file) - 1] = 0;
	} else if (!strcmp("image_size", token)) {
	    token = strtok(NULL, ",");
	    env->img_w = atoi(token);
	    token = strtok(NULL, ",");
	    env->img_h = atoi(token);
	    token = strtok(NULL, ",");
	    env->tile_w = atoi(token);
	    token = strtok(NULL, ",");
	    env->tile_h = atoi(token);
	} else if (!strcmp("hypersamples", token)) {
	    token = strtok(NULL, ",");
	    env->img_hs = atoi(token);
	} else if (!strcmp("rendering_method", token)) {
	    token = strtok(NULL, ",");
	    /* strip off newline */
	    if (token[strlen(token)-1] == '\n') token[strlen(token)-1] = 0;

	    if (!strcmp(token, "normal")) {
		env->rm = RENDER_METHOD_NORMAL;
		render_normal_init(&env->render);
	    } else if (!strcmp(token, "phong")) {
		env->rm = RENDER_METHOD_PHONG;
		render_phong_init(&env->render);
	    } else if (!strcmp(token, "depth")) {
		env->rm = RENDER_METHOD_DEPTH;
		render_depth_init(&env->render);
	    } else if (!strcmp(token, "path")) {
		env->rm = RENDER_METHOD_PATH;
		token = strtok(NULL, ",");
		render_path_init(&env->render, atoi(token));
	    } else if (!strcmp(token, "cut")) {
		TIE_3 pos, dir;
		int i;

		env->rm = RENDER_METHOD_PLANE;

		/* ray position */
		for (i = 0; i < 3; i++) {
		    token = strtok(NULL, ",");
		    pos.v[i] = atof(token);
		}

		/* ray direction */
		for (i = 0; i < 3; i++) {
		    token = strtok(NULL, ",");
		    dir.v[i] = atof(token);
		}

		render_cut_init(&env->render, pos, dir);
	    } else {
		env->rm = RENDER_METHOD_FLAT;
		render_flat_init(&env->render);
	    }

	}
    }

    fclose(fh);
}

/*
 * Local Variables:
 * mode: C
 * tab-width: 8
 * indent-tabs-mode: t
 * c-file-style: "stroustrup"
 * End:
 * ex: shiftwidth=4 tabstop=8
 */
