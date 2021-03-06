 /* fre:ac - free audio converter
  * Copyright (C) 2001-2017 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <dialogs/config/config_tags.h>

#include <config.h>

using namespace BoCA::AS;

freac::ConfigureTags::ConfigureTags()
{
	BoCA::Config	*config = BoCA::Config::Get();
	BoCA::I18n	*i18n	= BoCA::I18n::Get();

	i18n->SetContext("Configuration::Tags");

	enableCoverArtReadFromTags	= config->GetIntValue(Config::CategoryTagsID, Config::TagsCoverArtReadFromTagsID, Config::TagsCoverArtReadFromTagsDefault);
	enableCoverArtReadFromFiles	= config->GetIntValue(Config::CategoryTagsID, Config::TagsCoverArtReadFromFilesID, Config::TagsCoverArtReadFromFilesDefault);
	enableCoverArtWriteToTags	= config->GetIntValue(Config::CategoryTagsID, Config::TagsCoverArtWriteToTagsID, Config::TagsCoverArtWriteToTagsDefault);
	enableCoverArtWriteToFiles	= config->GetIntValue(Config::CategoryTagsID, Config::TagsCoverArtWriteToFilesID, Config::TagsCoverArtWriteToFilesDefault);
	enableCoverArtWriteToFilesRef	= config->GetIntValue(Config::CategoryTagsID, Config::TagsCoverArtWriteToFilesWithReferenceID, Config::TagsCoverArtWriteToFilesWithReferenceDefault);

	readCueSheets			= config->GetIntValue(Config::CategoryTagsID, Config::TagsReadEmbeddedCueSheetsID, Config::TagsReadEmbeddedCueSheetsDefault);
	preferCueSheets			= config->GetIntValue(Config::CategoryTagsID, Config::TagsPreferCueSheetsToChaptersID, Config::TagsPreferCueSheetsToChaptersDefault);

	readChapters			= config->GetIntValue(Config::CategoryTagsID, Config::TagsReadChaptersID, Config::TagsReadChaptersDefault);
	writeChapters			= config->GetIntValue(Config::CategoryTagsID, Config::TagsWriteChaptersID, Config::TagsWriteChaptersDefault);
	chapterFormat			= config->GetIntValue(Config::CategoryTagsID, Config::TagsWriteChaptersTypeID, Config::TagsWriteChaptersTypeDefault);

	writeMCDI			= config->GetIntValue(Config::CategoryTagsID, Config::TagsWriteMCDIID, Config::TagsWriteMCDIDefault);
	preserveReplayGain		= config->GetIntValue(Config::CategoryTagsID, Config::TagsPreserveReplayGainID, Config::TagsPreserveReplayGainDefault);

	prependZero			= False;
	replaceComments			= config->GetIntValue(Config::CategoryTagsID, Config::TagsReplaceExistingCommentsID, Config::TagsReplaceExistingCommentsDefault);

	tab_tags		= new TabWidget(Point(7, 7), Size(552, 213));

	layer_tags		= new Layer(i18n->TranslateString("Tags"));

	group_tags		= new GroupBox(i18n->TranslateString("Tag formats"), Point(7, 11), Size(534, 94));

	list_tag_formats	= new ListBox(Point(10, 13), Size(250, 71));
	list_tag_formats->SetFlags(LF_MULTICHECKBOX);

	Registry		&boca = Registry::Get();

	for (Int i = 0; i < boca.GetNumberOfComponents(); i++)
	{
		if (boca.GetComponentType(i) != BoCA::COMPONENT_TYPE_TAGGER) continue;

		const Array<TagSpec *>	&specs = boca.GetComponentTagSpecs(i);

		foreach (TagSpec *spec, specs)
		{
			list_tag_formats->AddEntry(spec->GetName(), config->GetIntValue(Config::CategoryTagsID, String("Enable").Append(spec->GetName().Replace(" ", NIL)), spec->IsDefault()));

			selected_encodings.Add(config->GetStringValue(Config::CategoryTagsID, spec->GetName().Replace(" ", NIL).Append("Encoding"), spec->GetDefaultEncoding()));
			tags_prependzero.Add(config->GetIntValue(Config::CategoryTagsID, String("TrackPrependZero").Append(spec->GetName().Replace(" ", NIL)), spec->IsPrependZeroDefault()));
		}
	}

	list_tag_formats->onSelectEntry.Connect(&ConfigureTags::ToggleTags, this);
	list_tag_formats->onMarkEntry.Connect(&ConfigureTags::ToggleTags, this);

	text_encoding		= new Text(i18n->AddColon(i18n->TranslateString("Encoding")), Point(268, 16));

	list_encodings		= new List();

	edit_encoding		= new EditBox(NIL, Point(text_encoding->GetUnscaledTextWidth() + 275, 13), Size(249 - text_encoding->GetUnscaledTextWidth(), 0));
	edit_encoding->onInput.Connect(&ConfigureTags::OnEditEncoding, this);

	combo_encoding		= new ComboBox(Point(text_encoding->GetUnscaledTextWidth() + 275, 13), Size(249 - text_encoding->GetUnscaledTextWidth(), 0));
	combo_encoding->onSelectEntry.Connect(&ConfigureTags::OnEditEncoding, this);
	combo_encoding->Hide();

	check_prependzero	= new CheckBox(i18n->TranslateString("Prepend zero to track numbers below 10"), Point(268, edit_encoding->GetY() + 28), Size(256, 0), &prependZero);
	check_prependzero->onAction.Connect(&ConfigureTags::TogglePrependZero, this);

	group_tags->Add(list_tag_formats);
	group_tags->Add(text_encoding);
	group_tags->Add(edit_encoding);
	group_tags->Add(combo_encoding);
	group_tags->Add(check_prependzero);

	group_definfo		= new GroupBox(i18n->TranslateString("Comments"), Point(7, 117), Size(534, 67));

	text_defcomment		= new Text(i18n->AddColon(i18n->TranslateString("Default comment string")), Point(10, 15));
	edit_defcomment		= new EditBox(config->GetStringValue(Config::CategoryTagsID, Config::TagsDefaultCommentID, NIL), Point(17 + text_defcomment->GetUnscaledTextWidth(), 12), Size(507 - text_defcomment->GetUnscaledTextWidth(), 0), 0);
	check_replace		= new CheckBox(i18n->TranslateString("Replace existing comments with default comment"), Point(10, edit_defcomment->GetY() + 28), Size(514, 0), &replaceComments);

	group_definfo->Add(text_defcomment);
	group_definfo->Add(edit_defcomment);
	group_definfo->Add(check_replace);

	layer_tags->Add(group_tags);
	layer_tags->Add(group_definfo);

	layer_coverart		= new Layer(i18n->TranslateString("Cover art"));

	group_coverart_read	= new GroupBox(i18n->TranslateString("Read cover art"), Point(7, 11), Size(534, 41));

	check_coverart_read_tags  = new CheckBox(i18n->TranslateString("Read cover art from tags"), Point(10, 14), Size(253, 0), &enableCoverArtReadFromTags);
	check_coverart_read_files = new CheckBox(i18n->TranslateString("Read cover art from files"), Point(271, 14), Size(253, 0), &enableCoverArtReadFromFiles);

	group_coverart_read->Add(check_coverart_read_tags);
	group_coverart_read->Add(check_coverart_read_files);

	group_coverart_write	= new GroupBox(i18n->TranslateString("Write cover art"), Point(7, 64), Size(534, 120));

	check_coverart_write_tags  = new CheckBox(i18n->TranslateString("Write cover art to tags"), Point(10, 14), Size(253, 0), &enableCoverArtWriteToTags);
	check_coverart_write_tags->onAction.Connect(&ConfigureTags::ToggleWriteCoverArt, this);

	list_coverart_write_tags_format = new ListBox(Point(27, 39), Size(236, 71));
	list_coverart_write_tags_format->SetFlags(LF_MULTICHECKBOX);

	for (Int i = 0; i < boca.GetNumberOfComponents(); i++)
	{
		if (boca.GetComponentType(i) != BoCA::COMPONENT_TYPE_TAGGER) continue;

		const Array<TagSpec *>	&specs = boca.GetComponentTagSpecs(i);

		foreach (TagSpec *spec, specs)
		{
			if (spec->IsCoverArtSupported()) list_coverart_write_tags_format->AddEntry(spec->GetName(), config->GetIntValue(Config::CategoryTagsID, String("CoverArtWriteTo").Append(spec->GetName().Replace(" ", NIL)), spec->IsCoverArtDefault()));
		}
	}

	check_coverart_write_files = new CheckBox(i18n->TranslateString("Write cover art to files"), Point(271, 14), Size(253, 0), &enableCoverArtWriteToFiles);
	check_coverart_write_files->onAction.Connect(&ConfigureTags::ToggleWriteCoverArt, this);

	text_coverart_write_files_name = new Text(i18n->AddColon(i18n->TranslateString("Filename pattern")), Point(288, 38));
	edit_coverart_write_files_name = new EditBox(config->GetStringValue(Config::CategoryTagsID, Config::TagsCoverArtFilenamePatternID, Config::TagsCoverArtFilenamePatternDefault), Point(288, 58), Size(236, 0), 0);

	check_coverart_write_files_ref = new CheckBox(i18n->TranslateString("Add reference to audio file tag"), Point(288, 86), Size(236, 0), &enableCoverArtWriteToFilesRef);

	group_coverart_write->Add(check_coverart_write_tags);
	group_coverart_write->Add(list_coverart_write_tags_format);
	group_coverart_write->Add(check_coverart_write_files);
	group_coverart_write->Add(text_coverart_write_files_name);
	group_coverart_write->Add(edit_coverart_write_files_name);
	group_coverart_write->Add(check_coverart_write_files_ref);

	layer_coverart->Add(group_coverart_read);
	layer_coverart->Add(group_coverart_write);

	layer_other		= new Layer(i18n->TranslateString("Other"));

	group_cue		= new GroupBox(i18n->TranslateString("Cue sheets"), Point(7, 11), Size(534, 67));

	check_read_cue		= new CheckBox(i18n->TranslateString("Read cue sheets embedded in metadata"), Point(10, 14), Size(514, 0), &readCueSheets);
	check_read_cue->onAction.Connect(&ConfigureTags::ToggleReadCueSheets, this);

	check_prefer_cue	= new CheckBox(i18n->TranslateString("Prefer cue sheets over chapter information"), Point(27, 40), Size(497, 0), &preferCueSheets);

	group_cue->Add(check_read_cue);
	group_cue->Add(check_prefer_cue);

	group_chapters		= new GroupBox(i18n->TranslateString("Chapters"), Point(7, 90), Size(263, 94));

	check_read_chapters	= new CheckBox(i18n->TranslateString("Read chapters from files"), Point(10, 14), Size(243, 0), &readChapters);
	check_write_chapters	= new CheckBox(i18n->TranslateString("Write chapters to files"), check_read_chapters->GetPosition() + Point(0, 26), Size(243, 0), &writeChapters);
	check_write_chapters->onAction.Connect(&ConfigureTags::ToggleWriteChapters, this);

	text_chapter_format	= new Text(i18n->AddColon(i18n->TranslateString("Chapter format")), check_write_chapters->GetPosition() + Point(17, 28));

	combo_chapter_format	= new ComboBox(text_chapter_format->GetPosition() + Point(text_chapter_format->GetUnscaledTextWidth() + 7, -3), Size(219 - text_chapter_format->GetUnscaledTextWidth(), 0));
	combo_chapter_format->AddEntry(i18n->TranslateString("both"));
	combo_chapter_format->AddEntry("QuickTime");
	combo_chapter_format->AddEntry("Nero");

	switch (chapterFormat)
	{
		default:
		case 1: // MP4ChapterTypeAny
			combo_chapter_format->SelectNthEntry(0);
			break;
		case 2: // MP4ChapterTypeQt
			combo_chapter_format->SelectNthEntry(1);
			break;
		case 4: // MP4ChapterTypeNero
			combo_chapter_format->SelectNthEntry(2);
			break;
	}

	group_chapters->Add(check_read_chapters);
	group_chapters->Add(check_write_chapters);
	group_chapters->Add(text_chapter_format);
	group_chapters->Add(combo_chapter_format);

	group_special		= new GroupBox(i18n->TranslateString("Special fields"), Point(278, 90), Size(263, 67));

	check_mcdi		= new CheckBox(i18n->TranslateString("Write CD table of contents"), Point(10, 14), Size(243, 0), &writeMCDI);
	check_replaygain	= new CheckBox(i18n->TranslateString("Preserve Replay Gain information"), check_mcdi->GetPosition() + Point(0, 26), Size(243, 0), &preserveReplayGain);

	group_special->Add(check_mcdi);
	group_special->Add(check_replaygain);

	layer_other->Add(group_cue);
	layer_other->Add(group_chapters);
	layer_other->Add(group_special);

	ToggleTags();

	ToggleWriteCoverArt();
	ToggleWriteChapters();

	ToggleReadCueSheets();

	tab_tags->Add(layer_tags);
	tab_tags->Add(layer_coverart);
	tab_tags->Add(layer_other);

	Add(tab_tags);

	SetSize(Size(566, 225));
}

freac::ConfigureTags::~ConfigureTags()
{
	DeleteObject(tab_tags);

	DeleteObject(layer_tags);

	DeleteObject(group_tags);
	DeleteObject(list_tag_formats);
	DeleteObject(text_encoding);
	DeleteObject(edit_encoding);
	DeleteObject(combo_encoding);
	DeleteObject(list_encodings);
	DeleteObject(check_prependzero);

	DeleteObject(layer_coverart);

	DeleteObject(group_coverart_read);
	DeleteObject(check_coverart_read_tags);
	DeleteObject(check_coverart_read_files);

	DeleteObject(group_coverart_write);
	DeleteObject(check_coverart_write_tags);
	DeleteObject(list_coverart_write_tags_format);
	DeleteObject(check_coverart_write_files);
	DeleteObject(text_coverart_write_files_name);
	DeleteObject(edit_coverart_write_files_name);
	DeleteObject(check_coverart_write_files_ref);

	DeleteObject(layer_other);

	DeleteObject(group_cue);
	DeleteObject(check_read_cue);
	DeleteObject(check_prefer_cue);

	DeleteObject(group_chapters);
	DeleteObject(check_read_chapters);
	DeleteObject(check_write_chapters);
	DeleteObject(text_chapter_format);
	DeleteObject(combo_chapter_format);

	DeleteObject(group_special);
	DeleteObject(check_mcdi);
	DeleteObject(check_replaygain);

	DeleteObject(group_definfo);
	DeleteObject(text_defcomment);
	DeleteObject(edit_defcomment);
	DeleteObject(check_replace);
}

Void freac::ConfigureTags::OnEditEncoding()
{
	if (edit_encoding->IsVisible())	selected_encodings.SetNth(list_tag_formats->GetSelectedEntryNumber(), edit_encoding->GetText());
	else				selected_encodings.SetNth(list_tag_formats->GetSelectedEntryNumber(), combo_encoding->GetSelectedEntry()->GetText());
}

Void freac::ConfigureTags::ToggleTags()
{
	ListEntry	*entry = list_tag_formats->GetSelectedEntry();

	if (entry == NIL)
	{
		edit_encoding->SetText(NIL);

		text_encoding->Deactivate();
		edit_encoding->Deactivate();

		check_prependzero->Deactivate();

		return;
	}

	/* Hide all controls first.
	 */
	edit_encoding->Hide();
	combo_encoding->Hide();

	list_encodings->RemoveAllEntries();
	combo_encoding->RemoveAllEntries();

	/* Configure and show controls according to format capabilities.
	 */
	Registry	&boca = Registry::Get();
	Int		 n = 0;

	for (Int i = 0; i < boca.GetNumberOfComponents(); i++)
	{
		if (boca.GetComponentType(i) != BoCA::COMPONENT_TYPE_TAGGER) continue;

		const Array<TagSpec *>	&specs = boca.GetComponentTagSpecs(i);

		foreach (TagSpec *spec, specs)
		{
			if (n++ != list_tag_formats->GetSelectedEntryNumber()) continue;

			const Array<String>	&encodings = spec->GetEncodings();

			foreach (const String &encoding, encodings)
			{
				list_encodings->AddEntry(encoding);
				combo_encoding->AddEntry(encoding);
			}

			if (spec->IsFreeEncodingSupported())
			{
				edit_encoding->SetText(selected_encodings.GetNth(n - 1));
				edit_encoding->Show();
			}
			else
			{
				combo_encoding->SelectEntry(selected_encodings.GetNth(n - 1));
				combo_encoding->Show();
			}

			check_prependzero->SetChecked(tags_prependzero.GetNth(n - 1));

			if (spec->IsPrependZeroAllowed()) check_prependzero->Activate();
			else				  check_prependzero->Deactivate();

			break;
		}

		if (n > list_tag_formats->GetSelectedEntryNumber()) break;
	}

	edit_encoding->SetDropDownList(list_encodings);

	/* Activate encoding controls if more than one encoding is available.
	 */
	if (list_encodings->Length() > 1)
	{
		text_encoding->Activate();
		edit_encoding->Activate();
		combo_encoding->Activate();
	}
	else
	{
		text_encoding->Deactivate();
		edit_encoding->Deactivate();
		combo_encoding->Deactivate();
	}
}

Void freac::ConfigureTags::TogglePrependZero()
{
	tags_prependzero.SetNth(list_tag_formats->GetSelectedEntryNumber(), prependZero);
}

Void freac::ConfigureTags::ToggleWriteCoverArt()
{
	if (enableCoverArtWriteToTags)	list_coverart_write_tags_format->Activate();
	else				list_coverart_write_tags_format->Deactivate();

	if (enableCoverArtWriteToFiles)
	{
		text_coverart_write_files_name->Activate();
		edit_coverart_write_files_name->Activate();
		check_coverart_write_files_ref->Activate();
	}
	else
	{
		text_coverart_write_files_name->Deactivate();
		edit_coverart_write_files_name->Deactivate();
		check_coverart_write_files_ref->Deactivate();
	}
}

Void freac::ConfigureTags::ToggleWriteChapters()
{
	if (writeChapters)
	{
		text_chapter_format->Activate();
		combo_chapter_format->Activate();
	}
	else
	{
		text_chapter_format->Deactivate();
		combo_chapter_format->Deactivate();
	}
}

Void freac::ConfigureTags::ToggleReadCueSheets()
{
	if (readCueSheets) check_prefer_cue->Activate();
	else		   check_prefer_cue->Deactivate();
}

Int freac::ConfigureTags::SaveSettings()
{
	BoCA::Config	*config = BoCA::Config::Get();
	Registry	&boca = Registry::Get();

	for (Int i = 0; i < boca.GetNumberOfComponents(); i++)
	{
		if (boca.GetComponentType(i) != BoCA::COMPONENT_TYPE_TAGGER) continue;

		const Array<TagSpec *>	&specs = boca.GetComponentTagSpecs(i);

		foreach (TagSpec *spec, specs)
		{
			config->SetIntValue(Config::CategoryTagsID, String("Enable").Append(spec->GetName().Replace(" ", NIL)), list_tag_formats->GetEntry(spec->GetName())->IsMarked());
			config->SetStringValue(Config::CategoryTagsID, spec->GetName().Replace(" ", NIL).Append("Encoding"), selected_encodings.GetNth(list_tag_formats->GetEntryNumber(spec->GetName())));

			if (spec->IsPrependZeroAllowed()) config->SetIntValue(Config::CategoryTagsID, String("TrackPrependZero").Append(spec->GetName().Replace(" ", NIL)), tags_prependzero.GetNth(list_tag_formats->GetEntryNumber(spec->GetName())));
			if (spec->IsCoverArtSupported())  config->SetIntValue(Config::CategoryTagsID, String("CoverArtWriteTo").Append(spec->GetName().Replace(" ", NIL)), list_coverart_write_tags_format->GetEntry(spec->GetName())->IsMarked());
		}
	}

	config->SetIntValue(Config::CategoryTagsID, Config::TagsCoverArtReadFromTagsID, enableCoverArtReadFromTags);
	config->SetIntValue(Config::CategoryTagsID, Config::TagsCoverArtReadFromFilesID, enableCoverArtReadFromFiles);
	config->SetIntValue(Config::CategoryTagsID, Config::TagsCoverArtWriteToTagsID, enableCoverArtWriteToTags);
	config->SetIntValue(Config::CategoryTagsID, Config::TagsCoverArtWriteToFilesID, enableCoverArtWriteToFiles);
	config->SetIntValue(Config::CategoryTagsID, Config::TagsCoverArtWriteToFilesWithReferenceID, enableCoverArtWriteToFilesRef);

	config->SetStringValue(Config::CategoryTagsID, Config::TagsCoverArtFilenamePatternID, edit_coverart_write_files_name->GetText());

	config->SetIntValue(Config::CategoryTagsID, Config::TagsReadEmbeddedCueSheetsID, readCueSheets);
	config->SetIntValue(Config::CategoryTagsID, Config::TagsPreferCueSheetsToChaptersID, preferCueSheets);

	config->SetIntValue(Config::CategoryTagsID, Config::TagsReadChaptersID, readChapters);
	config->SetIntValue(Config::CategoryTagsID, Config::TagsWriteChaptersID, writeChapters);

	switch (combo_chapter_format->GetSelectedEntryNumber())
	{
		default:
		case 0: // Any
			config->SetIntValue(Config::CategoryTagsID, Config::TagsWriteChaptersTypeID, 1);
			break;
		case 1: // QuickTime
			config->SetIntValue(Config::CategoryTagsID, Config::TagsWriteChaptersTypeID, 2);
			break;
		case 2: // Nero
			config->SetIntValue(Config::CategoryTagsID, Config::TagsWriteChaptersTypeID, 4);
			break;
	}

	config->SetIntValue(Config::CategoryTagsID, Config::TagsWriteMCDIID, writeMCDI);
	config->SetIntValue(Config::CategoryTagsID, Config::TagsPreserveReplayGainID, preserveReplayGain);

	config->SetStringValue(Config::CategoryTagsID, Config::TagsDefaultCommentID, edit_defcomment->GetText());
	config->SetIntValue(Config::CategoryTagsID, Config::TagsReplaceExistingCommentsID, replaceComments);

	return Success();
}
