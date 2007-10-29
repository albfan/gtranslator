/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * prefs-manager.h
 * This file is part of gtranslator based in gedit
 *
 * Copyright (C) 2002  Paolo Maggi 
 * 		 2007  Ignacio Casal Quinteiro
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, 
 * Boston, MA 02111-1307, USA. 
 */

#ifndef __GTR_PREFS_MANAGER_H__
#define __GTR_PREFS_MANAGER_H__

//#include "application.h"
#include <glib.h>

#define GTR_BASE_KEY			"/apps/gtranslator"

#define GPM_PREFS_DIR			GTR_BASE_KEY "/preferences"

/* Files */
#define GPM_GENERAL_DIR			GPM_PREFS_DIR "/files/general"
#define GPM_WARN_IF_FUZZY		GPM_GENERAL_DIR "/warn_if_fuzzy"
#define GPM_SWEEP_COMPILE_FILE		GPM_GENERAL_DIR "/sweep_compile_file"

#define GPM_AUTOSAVE_DIR		GPM_PREFS_DIR "/files/autosave"
#define GPM_AUTOSAVE			GPM_AUTOSAVE_DIR "/autosave"
#define GPM_AUTOSAVE_TIMEOUT		GPM_AUTOSAVE_DIR "/autosave_timeout"
#define GPM_AUTOSAVE_WITH_SUFFIX	GPM_AUTOSAVE_DIR "/autosave_with_suffix"
#define GPM_AUTOSAVE_SUFFIX		GPM_AUTOSAVE_DIR "/autosave_suffix"

/*With the new recent files implementation this is not neccessary*/
//#define GPM_RECENT_DIR			GPM_PREFS_DIR "/files/recent_files"

/* Editor */
#define GPM_TEXT_DISPLAY_DIR		GPM_PREFS_DIR "/editor/text_display"
#define GPM_HIGHLIGHT			GPM_TEXT_DISPLAY_DIR "/highlight"
#define GPM_USE_DOT_CHAR		GPM_TEXT_DISPLAY_DIR "/user_dot_char"
#define GPM_FONT_DIR			GPM_TEXT_DISPLAY_DIR "/fonts"
#define GPM_OWN_FONTS			GPM_FONT_DIR "/own_fonts"
#define GPM_TEXT_FONT			GPM_FONT_DIR "/text_font"

#define GPM_CONTENTS_DIR		GPM_PREFS_DIR "/editor/contents"
#define GPM_UNMARK_FUZZY		GPM_CONTENTS_DIR "/unmark_fuzzy"
#define GPM_KEEP_OBSOLETE		GPM_CONTENTS_DIR "/keep_obsolete"
#define GPM_INSTANT_SPELL_CHECKING	GPM_CONTENTS_DIR "/instant_spell_checking"

/* PO header */
#define GPM_PERSONAL_INFORMATION_DIR	GPM_PREFS_DIR "/po_header/personal_information"
#define GPM_AUTHORS_NAME		GPM_PERSONAL_INFORMATION_DIR "authors_name"
#define GPM_AUTHORS_EMAIL		GPM_PERSONAL_INFORMATION_DIR "authors_email"

#define GPM_LANGUAGE_SETTINGS_DIR	GPM_PREFS_DIR "/po_header/language_settings"
#define GPM_AUTHORS_LANGUAGE		GPM_LANGUAGE_SETTINGS_DIR "/authors_language"
#define GPM_LCODE			GPM_LANGUAGE_SETTINGS_DIR "/lcode"
#define GPM_MIME_TYPE			GPM_LANGUAGE_SETTINGS_DIR "/mime_type"
#define GPM_ENCODING			GPM_LANGUAGE_SETTINGS_DIR "/encoding"
#define GPM_LG_EMAIL			GPM_LANGUAGE_SETTINGS_DIR "/lg_email"
#define GPM_NUMBER_PLURALS		GPM_LANGUAGE_SETTINGS_DIR "/number_plurals"
#define GPM_PLURAL			GPM_LANGUAGE_SETTINGS_DIR "/plural"
#define GPM_PLURAL_NOTE			GPM_LANGUAGE_SETTINGS_DIR "/plural_note"


/* Fallback default values. Keep in sync with gtranslator.schemas */

/* Files */
#define GPM_DEFAULT_WARN_IF_FUZZY	  0 /* FALSE */
#define GPM_DEFAULT_SWEEP_COMPILE_FILE    0 /* FALSE */

#define GPM_DEFAULT_AUTOSAVE		  0 /* FALSE */
#define GPM_DEFAULT_AUTOSAVE_TIMEOUT	  4
#define GPM_DEFAULT_AUTOSAVE_WITH_SUFFIX  0 /* FALSE */
#define GPM_DEFAULT_AUTOSAVE_SUFFIX	  (const gchar*) "autosave"

/* Editor */
#define GPM_DEFAULT_HIGHLIGHT             1 /* TRUE */
#define GPM_DEFAULT_USE_DOT_CHAR          1 /* TRUE */
#define GPM_DEFAULT_OWN_FONTS             0 /* FALSE */
#define GPM_DEFAULT_TEXT_FONT             (const gchar*) "Sans 10"

#define GPM_DEFAULT_UNMARK_FUZZY          0 /* FALSE */
#define GPM_DEFAULT_KEEP_OBSOLETE         0 /* FALSE */
#define GPM_DEFAULT_INSTANT_SPELL_CHECKING 1 /* TRUE */

/* PO header */
#define GPM_DEFAULT_AUTHORS_NAME          (const gchar*) ""
#define GPM_DEFAULT_AUTHORS_EMAIL         (const gchar*) ""

/*#define GPM_AUTHORS_LANGUAGE		GPM_LANGUAGE_SETTINGS_DIR "/authors_language"
#define GPM_LCODE			GPM_LANGUAGE_SETTINGS_DIR "/lcode"
#define GPM_MIME_TYPE			GPM_LANGUAGE_SETTINGS_DIR "/mime_type"
#define GPM_ENCODING			GPM_LANGUAGE_SETTINGS_DIR "/encoding"
#define GPM_LG_EMAIL			GPM_LANGUAGE_SETTINGS_DIR "/lg_email"*/
#define GPM_DEFAULT_NUMBER_PLURALS      2
#define GPM_DEFAULT_PLURAL              (const gchar*) ""
#define GPM_DEFAULT_PLURAL_NOTE         ""

/** LIFE CYCLE MANAGEMENT FUNCTIONS **/

gboolean		 gtranslator_prefs_manager_init                         (void);

/* This function must be called before exiting gtranslator */
void			 gtranslator_prefs_manager_shutdown                     (void);


/** PREFS MANAGEMENT FUNCTIONS **/

/* Warn if fuzzy*/
void                     gtranslator_prefs_manager_set_warn_if_fuzzy            (gboolean wif);
gboolean                 gtranslator_prefs_manager_get_warn_if_fuzzy            (void);

/*Sweep compile file*/
void                     gtranslator_prefs_manager_set_sweep_compile_file       (gboolean scf);
gboolean                 gtranslator_prefs_manager_get_sweep_compile_file       (void);

/*Autosave*/
void                     gtranslator_prefs_manager_set_autosave                 (gboolean autosave);
gboolean                 gtranslator_prefs_manager_get_autosave                 (void);

/*Autosave with suffix*/
void                     gtranslator_prefs_manager_set_autosave_with_suffix     (gboolean aws);
gboolean                 gtranslator_prefs_manager_get_autosave_with_suffix     (void);

/*Autosave timeout*/
void                     gtranslator_prefs_manager_set_autosave_timeout         (gint timeout);
gint                     gtranslator_prefs_manager_get_autosave_timeout         (void);

/*Autosave suffix*/
void                     gtranslator_prefs_manager_set_autosave_suffix          (const gchar *suffix);
const gchar *            gtranslator_prefs_manager_get_autosave_suffix          (void);

/*Highlight*/
void                     gtranslator_prefs_manager_set_highlight                (gboolean highlight);
gboolean                 gtranslator_prefs_manager_get_highlight                (void);

/*Use dot char*/
void                     gtranslator_prefs_manager_set_use_dot_char             (gboolean udc);
gboolean                 gtranslator_prefs_manager_get_use_dot_char             (void);

/*Own fonts*/
void                     gtranslator_prefs_manager_set_own_fonts                (gboolean own_fonts);
gboolean                 gtranslator_prefs_manager_get_own_fonts                (void);

/*Text font*/
void                     gtranslator_prefs_manager_set_text_font                (const gchar *font);
const gchar *            gtranslator_prefs_manager_get_text_font                (void);

/*Unmark fuzzy*/
void                     gtranslator_prefs_manager_set_unmark_fuzzy             (gboolean unmark);
gboolean                 gtranslator_prefs_manager_get_unmark_fuzzy             (void);

/*keep obsolete*/
void                     gtranslator_prefs_manager_set_keep_obsolete            (gboolean keep_obsolete);
gboolean                 gtranslator_prefs_manager_get_keep_obsolete            (void);

/*Instant spell checking*/
void                     gtranslator_prefs_manager_set_instant_spell_checking   (gboolean spell_checking);
gboolean                 gtranslator_prefs_manager_get_instant_spell_checking   (void);

/*authors name*/
void                     gtranslator_prefs_manager_set_authors_name             (const gchar *name);
const gchar *            gtranslator_prefs_manager_get_authors_name             (void);

/*authors email*/
void                     gtranslator_prefs_manager_set_authors_email            (const gchar *email);
const gchar *            gtranslator_prefs_manager_get_authors_email            (void);


#endif  /* __GTR_PREFS_MANAGER_H__ */


