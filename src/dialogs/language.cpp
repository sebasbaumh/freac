 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2005 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <dialogs/language.h>
#include <resources.h>

languageDlg::languageDlg()
{
	currentConfig = bonkEnc::currentConfig;

	Point	 pos;
	Size	 size;

	mainWnd			= new Window(String("Welcome to BonkEnc ").Append(bonkEnc::shortVersion));
	mainWnd_titlebar	= new Titlebar(TB_NONE);
	divbar			= new Divider(42, OR_HORZ | OR_BOTTOM);

	pos.x = 87;
	pos.y = 29;
	size.cx = 0;
	size.cy = 0;

	btn_ok			= new Button("OK", NIL, pos, size);
	btn_ok->onClick.Connect(&languageDlg::OK, this);
	btn_ok->SetOrientation(OR_LOWERRIGHT);

	pos.x = 6;
	pos.y = 5;

	text_language	= new Text("Select your language:", pos);

	pos.x = 7;
	pos.y += 19;
	size.cx = 280;
	size.cy = 94;

	list_language	= new ListBox(pos, size);

	for (int i = 0; i < bonkEnc::i18n->GetNOfLanguages(); i++)
	{
		list_language->AddEntry(bonkEnc::i18n->GetNthLanguageName(i));

		if (bonkEnc::i18n->GetNthLanguageID(i) == "english-internal") list_language->SelectNthEntry(i);
	}

	RegisterObject(mainWnd);

	mainWnd->RegisterObject(btn_ok);
	mainWnd->RegisterObject(text_language);
	mainWnd->RegisterObject(list_language);
	mainWnd->RegisterObject(mainWnd_titlebar);
	mainWnd->RegisterObject(divbar);

	mainWnd->SetIcon(Bitmap::LoadBitmap("bonkenc.pci", 0, NIL));
	mainWnd->SetMetrics(Point(120, 120), Size(300, 196));
}

languageDlg::~languageDlg()
{
	delete mainWnd_titlebar;
	delete mainWnd;
	delete divbar;
	delete text_language;
	delete list_language;
	delete btn_ok;
}

Int languageDlg::ShowDialog()
{
	mainWnd->Stay();

	return Success;
}

Void languageDlg::OK()
{
	if (list_language->GetSelectedEntry() != NIL) currentConfig->language = bonkEnc::i18n->GetNthLanguageID(list_language->GetSelectedEntryNumber());

	mainWnd->Close();
}