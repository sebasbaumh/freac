 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2012 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <dialogs/error.h>
#include <resources.h>

BonkEnc::ErrorDialog::ErrorDialog(const Array<String> &errors)
{
	BoCA::I18n	*i18n = BoCA::I18n::Get();

	i18n->SetContext("Dialogs::Errors");

	Rect	 workArea = S::System::MultiMonitor::GetActiveMonitorWorkArea();

	mainWnd			= new Window(i18n->TranslateString("Errors"), workArea.GetPosition() + Point((workArea.GetSize().cx - 450) / 2, (workArea.GetSize().cy - 400) / 2), Size(450, 400));
	mainWnd->SetMinimumSize(Size(330, 220));
	mainWnd->SetRightToLeft(i18n->IsActiveLanguageRightToLeft());
	mainWnd->GetMainLayer()->onChangeSize.Connect(&ErrorDialog::OnChangeSize, this);

	mainWnd_titlebar	= new Titlebar(TB_CLOSEBUTTON);
	divbar			= new Divider(39, OR_HORZ | OR_BOTTOM);

	btn_ok			= new Button(i18n->TranslateString("OK"), NIL, Point(87, 29), Size());
	btn_ok->onAction.Connect(&ErrorDialog::OK, this);
	btn_ok->SetOrientation(OR_LOWERRIGHT);

	text_errors	= new Text(String(i18n->TranslateString("%1 errors while processing job")).Replace("%1", String::FromInt(errors.Length())).Append(":"), Point(7, 5));

	list_errors	= new ListBox(Point(7, 24), Size(378, 180));
	list_errors->onSelectEntry.Connect(&ErrorDialog::OnSelectError, this);

	foreach (const String &error, errors) list_errors->AddEntry(error);

	edit_details	= new MultiEdit(NIL, Point(7, 104), Size(378, 57));
	edit_details->SetOrientation(OR_LOWERLEFT);
	edit_details->Deactivate();

	text_details	= new Text(i18n->TranslateString("select an entry for details"), Point());
	text_details->SetFont(GUI::Font(GUI::Font::Default, 12, GUI::Font::Bold, 0, Setup::GrayTextColor));
	text_details->SetOrientation(OR_LOWERLEFT);

	Add(mainWnd);

	mainWnd->Add(btn_ok);

	mainWnd->Add(text_errors);
	mainWnd->Add(list_errors);

	mainWnd->Add(edit_details);
	mainWnd->Add(text_details);

	mainWnd->Add(mainWnd_titlebar);
	mainWnd->Add(divbar);

	mainWnd->SetFlags(WF_NORMAL);
	mainWnd->SetIcon(ImageLoader::Load("icons/freac.png"));
}

BonkEnc::ErrorDialog::~ErrorDialog()
{
	DeleteObject(mainWnd_titlebar);
	DeleteObject(mainWnd);
	DeleteObject(divbar);

	DeleteObject(text_errors);
	DeleteObject(list_errors);

	DeleteObject(edit_details);
	DeleteObject(text_details);

	DeleteObject(btn_ok);
}

const Error &BonkEnc::ErrorDialog::ShowDialog()
{
	mainWnd->Stay();

	return error;
}

Void BonkEnc::ErrorDialog::OnChangeSize(const Size &nSize)
{
	Rect	 clientRect = Rect(mainWnd->GetMainLayer()->GetPosition(), mainWnd->GetMainLayer()->GetSize());
	Size	 clientSize = Size(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top);

	list_errors->SetSize(clientSize - Size(14, 136));

	edit_details->SetWidth(clientSize.cx - 14);
	text_details->SetPosition(edit_details->GetPosition() + Point((edit_details->GetWidth() - text_details->GetUnscaledTextWidth()) / 2, (-(edit_details->GetHeight() - text_details->GetUnscaledTextHeight())) / 2));
}

Void BonkEnc::ErrorDialog::OK()
{
	mainWnd->Close();
}

Void BonkEnc::ErrorDialog::OnSelectError(ListEntry *error)
{
	text_details->Hide();
	edit_details->SetText(error->GetText());
}