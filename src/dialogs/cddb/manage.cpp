 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2006 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <dialogs/cddb/manage.h>
#include <resources.h>
#include <dllinterfaces.h>
#include <joblist.h>
#include <cddb/cddb.h>
#include <utilities.h>

BonkEnc::cddbManageDlg::cddbManageDlg()
{
	currentConfig	= BonkEnc::currentConfig;

	updateJoblist	= currentConfig->update_joblist;

	Point	 pos;
	Size	 size;

	mainWnd			= new Window(BonkEnc::i18n->TranslateString("CDDB data"), Point(120, 120), Size(550, 350));
	mainWnd->SetRightToLeft(BonkEnc::i18n->IsActiveLanguageRightToLeft());

	mainWnd_titlebar	= new Titlebar(TB_CLOSEBUTTON);
	divbar			= new Divider(42, OR_HORZ | OR_BOTTOM);

	pos.x = 175;
	pos.y = 29;
	size.cx = 0;
	size.cy = 0;

	btn_cancel		= new Button(BonkEnc::i18n->TranslateString("Cancel"), NIL, pos, size);
	btn_cancel->onAction.Connect(&cddbManageDlg::Cancel, this);
	btn_cancel->SetOrientation(OR_LOWERRIGHT);

	pos.x -= 88;

	btn_ok		= new Button(BonkEnc::i18n->TranslateString("OK"), NIL, pos, size);
	btn_ok->onAction.Connect(&cddbManageDlg::OK, this);
	btn_ok->SetOrientation(OR_LOWERRIGHT);

	pos.x = 7;
	pos.y = 27;

	check_updateJoblist	= new CheckBox(BonkEnc::i18n->TranslateString("Update joblist with this information"), pos, size, &updateJoblist);
	check_updateJoblist->SetWidth(check_updateJoblist->textSize.cx + 21);
	check_updateJoblist->SetOrientation(OR_LOWERLEFT);

	pos.x = 7;
	pos.y = 10;

	text_entries	= new Text(BonkEnc::i18n->TranslateString("Downloaded CDDB entries:"), pos);

	pos.y += 19;
	size.cx = 261;
	size.cy = 213;

	list_entries	= new ListBox(pos, size);
	list_entries->AddTab(BonkEnc::i18n->TranslateString("Disc name"), 0);
	list_entries->AddTab(BonkEnc::i18n->TranslateString("Charset"), 100);
	list_entries->onSelectEntry.Connect(&cddbManageDlg::SelectEntry, this);

	for (Int i = 0; i < CDDB::infoCache.GetNOfEntries(); i++)
	{
		CDDBInfo	*entry = CDDB::infoCache.GetNthEntry(i);

		list_entries->AddEntry(String(entry->dArtist).Append(" - ").Append(entry->dTitle).Append("\t").Append(entry->charset));
	}

	pos.x += 269;
	pos.y -= 19;

	text_preview	= new Text(String(BonkEnc::i18n->TranslateString("Preview")).Append(":"), pos);

	pos.y += 19;

	size.cx = 261;
	size.cy = 186;

	edit_preview	= new MultiEdit("", pos, size, 0);
	edit_preview->Deactivate();

	pos.y += 197;

	text_charset	= new Text(String(BonkEnc::i18n->TranslateString("Charset")).Append(":"), pos);

	pos.x += (text_charset->textSize.cx + 7);
	pos.y -= 3;
	size.cx = 254 - text_charset->textSize.cx;
	size.cy = 0;

	list_charset	= new ListBox(pos, size);
	list_charset->AddEntry("CP1251");
	list_charset->AddEntry("ISO-8859-1");
	list_charset->AddEntry("ISO-8859-2");
	list_charset->AddEntry("ISO-8859-5");
	list_charset->AddEntry("ISO-8859-7");
	list_charset->AddEntry("SHIFT-JIS");
	list_charset->AddEntry("GBK");
	list_charset->AddEntry("BIG-5");

	edit_charset	= new EditBox("", pos, size, 0);
	edit_charset->SetDropDownList(list_charset);
	edit_charset->onInput.Connect(&cddbManageDlg::SetCharset, this);

	pos.x = 188;
	pos.y = 69;
	size.cx = 0;
	size.cy = 0;

	btn_delete	= new Button(BonkEnc::i18n->TranslateString("Remove entry"), NIL, pos, size);
	btn_delete->onAction.Connect(&cddbManageDlg::DeleteEntry, this);
	btn_delete->SetOrientation(OR_LOWERLEFT);

	pos.x = 457;

	btn_save	= new Button(BonkEnc::i18n->TranslateString("Save entry"), NIL, pos, size);
	btn_save->onAction.Connect(&cddbManageDlg::SaveEntry, this);
	btn_save->SetOrientation(OR_LOWERLEFT);

	edit_charset->Deactivate();
	btn_delete->Deactivate();
	btn_save->Deactivate();

	RegisterObject(mainWnd);

	mainWnd->RegisterObject(btn_ok);
	mainWnd->RegisterObject(btn_cancel);
	mainWnd->RegisterObject(check_updateJoblist);
	mainWnd->RegisterObject(mainWnd_titlebar);
	mainWnd->RegisterObject(divbar);
	mainWnd->RegisterObject(text_entries);
	mainWnd->RegisterObject(list_entries);
	mainWnd->RegisterObject(text_charset);
	mainWnd->RegisterObject(edit_charset);
	mainWnd->RegisterObject(text_preview);
	mainWnd->RegisterObject(edit_preview);
	mainWnd->RegisterObject(btn_delete);
	mainWnd->RegisterObject(btn_save);

	mainWnd->SetFlags(mainWnd->GetFlags() | WF_NOTASKBUTTON);
	mainWnd->SetIcon(ImageLoader::Load("BonkEnc.pci:0"));
}

BonkEnc::cddbManageDlg::~cddbManageDlg()
{
	DeleteObject(mainWnd_titlebar);
	DeleteObject(mainWnd);
	DeleteObject(divbar);
	DeleteObject(btn_ok);
	DeleteObject(btn_cancel);

	DeleteObject(text_entries);
	DeleteObject(list_entries);

	DeleteObject(text_charset);
	DeleteObject(edit_charset);
	DeleteObject(list_charset);
	DeleteObject(text_preview);
	DeleteObject(edit_preview);

	DeleteObject(btn_delete);
	DeleteObject(btn_save);

	DeleteObject(check_updateJoblist);
}

const Error &BonkEnc::cddbManageDlg::ShowDialog()
{
	mainWnd->Stay();

	return error;
}

Void BonkEnc::cddbManageDlg::OK()
{
	if (updateJoblist)
	{
		for (Int i = 0; i < CDDB::infoCache.GetNOfEntries(); i++)
		{
			CDDBInfo	*cddbInfo = CDDB::infoCache.GetNthEntry(i);

			for (Int l = 0; l < currentConfig->appMain->joblist->GetNOfTracks(); l++)
			{
				Track	*trackInfo = currentConfig->appMain->joblist->GetNthTrack(l);

				if (trackInfo->discid != cddbInfo->DiscIDToString()) continue;

				for (Int m = 0; m < cddbInfo->trackTitles.GetNOfEntries(); m++)
				{
					if (trackInfo->cdTrack == m + 1)
					{
						trackInfo->artist	= (cddbInfo->dArtist == "Various" ? cddbInfo->trackArtists.GetNthEntry(m) : cddbInfo->dArtist);
						trackInfo->title	= cddbInfo->trackTitles.GetNthEntry(m);
						trackInfo->album	= cddbInfo->dTitle;
						trackInfo->year		= cddbInfo->dYear;
						trackInfo->genre	= cddbInfo->dGenre;
						trackInfo->comment	= cddbInfo->trackComments.GetNthEntry(m);

						String	 jlEntry;

						if (trackInfo->artist == NIL && trackInfo->title == NIL)	jlEntry = String(trackInfo->origFilename).Append("\t");
						else								jlEntry = String(trackInfo->artist.Length() > 0 ? trackInfo->artist : currentConfig->appMain->i18n->TranslateString("unknown artist")).Append(" - ").Append(trackInfo->title.Length() > 0 ? trackInfo->title : currentConfig->appMain->i18n->TranslateString("unknown title")).Append("\t");

						jlEntry.Append(trackInfo->track > 0 ? (trackInfo->track < 10 ? String("0").Append(String::FromInt(trackInfo->track)) : String::FromInt(trackInfo->track)) : String("")).Append("\t").Append(trackInfo->lengthString).Append("\t").Append(trackInfo->fileSizeString);

						if (currentConfig->appMain->joblist->GetNthEntry(l)->GetText() != jlEntry) currentConfig->appMain->joblist->GetNthEntry(l)->SetText(jlEntry);
					}
				}
			}
		}
	}

	currentConfig->update_joblist = updateJoblist;

	mainWnd->Close();
}

Void BonkEnc::cddbManageDlg::Cancel()
{
	mainWnd->Close();
}

Void BonkEnc::cddbManageDlg::SetCharset()
{
	CDDBInfo	*entry = CDDB::infoCache.GetNthEntry(list_entries->GetSelectedEntryNumber());
	String		 dArtist;
	String		 dTitle;

	dArtist.ImportFrom(edit_charset->GetText(), entry->oDArtist);
	dTitle.ImportFrom(edit_charset->GetText(), entry->oDTitle);

	String		 preview = String(dArtist).Append(" - ").Append(dTitle).Append("\n\n");

	for (Int i = 0; i < entry->oTrackTitles.GetNOfEntries(); i++)
	{
		String	 artist;
		String	 title;

		artist.ImportFrom(edit_charset->GetText(), entry->oTrackArtists.GetNthEntry(i));
		title.ImportFrom(edit_charset->GetText(), entry->oTrackTitles.GetNthEntry(i));

		preview.Append(i < 9 ? "0" : "").Append(String::FromInt(i + 1)).Append(": ").Append(entry->oDArtist == "Various" ? String(artist).Append(" - ") : "").Append(title).Append("\n");
	}

	edit_preview->SetText(preview);
}

Void BonkEnc::cddbManageDlg::SelectEntry()
{
	CDDBInfo	*entry = CDDB::infoCache.GetNthEntry(list_entries->GetSelectedEntryNumber());
	String		 preview = String(entry->dArtist).Append(" - ").Append(entry->dTitle).Append("\n\n");

	for (Int i = 0; i < entry->trackTitles.GetNOfEntries(); i++)
	{
		preview.Append(i < 9 ? "0" : "").Append(String::FromInt(i + 1)).Append(": ").Append(entry->dArtist == "Various" ? String(entry->trackArtists.GetNthEntry(i)).Append(" - ") : "").Append(entry->trackTitles.GetNthEntry(i)).Append("\n");
	}

	edit_preview->SetText(preview);
	edit_charset->SetText(entry->charset);

	edit_charset->Activate();
	btn_delete->Activate();
	btn_save->Activate();
}

Void BonkEnc::cddbManageDlg::DeleteEntry()
{
	CDDB::infoCache.RemoveEntry(CDDB::infoCache.GetNthEntryIndex(list_entries->GetSelectedEntryNumber()));

	list_entries->RemoveEntry(list_entries->GetSelectedEntry());

	edit_preview->SetText("");

	edit_charset->Deactivate();
	btn_delete->Deactivate();
	btn_save->Deactivate();
}

Void BonkEnc::cddbManageDlg::SaveEntry()
{
	CDDBInfo	*entry = CDDB::infoCache.GetNthEntry(list_entries->GetSelectedEntryNumber());

	entry->dArtist.ImportFrom(edit_charset->GetText(), entry->oDArtist);
	entry->dTitle.ImportFrom(edit_charset->GetText(), entry->oDTitle);
	entry->dGenre.ImportFrom(edit_charset->GetText(), entry->oDGenre);
	entry->comment.ImportFrom(edit_charset->GetText(), entry->oComment);

	for (Int i = 0; i < entry->trackArtists.GetNOfEntries(); i++)
	{
		String	 artist;
		String	 title;
		String	 comment;

		artist.ImportFrom(edit_charset->GetText(), entry->oTrackArtists.GetNthEntry(i));
		title.ImportFrom(edit_charset->GetText(), entry->oTrackTitles.GetNthEntry(i));
		comment.ImportFrom(edit_charset->GetText(), entry->oTrackComments.GetNthEntry(i));

		entry->trackArtists.SetEntry(entry->trackArtists.GetNthEntryIndex(i), artist);
		entry->trackTitles.SetEntry(entry->trackTitles.GetNthEntryIndex(i), title);
		entry->trackComments.SetEntry(entry->trackComments.GetNthEntryIndex(i), comment);
	}

	entry->charset = edit_charset->GetText();

	list_entries->GetSelectedEntry()->SetText(String(entry->dArtist).Append(" - ").Append(entry->dTitle).Append("\t").Append(entry->charset));
}
