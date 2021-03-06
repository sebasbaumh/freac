 /* fre:ac - free audio converter
  * Copyright (C) 2001-2016 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <jobs/joblist/removedisc.h>

#include <boca.h>

using namespace BoCA;

freac::JobRemoveDiscTracks::JobRemoveDiscTracks(Int iDrive)
{
	drive = iDrive;
}

freac::JobRemoveDiscTracks::~JobRemoveDiscTracks()
{
}

Bool freac::JobRemoveDiscTracks::ReadyToRun()
{
	if (!BoCA::JobList::Get()->IsLocked())
	{
		BoCA::JobList::Get()->Lock();

		return True;
	}

	return False;
}

Error freac::JobRemoveDiscTracks::Perform()
{
	SetText(String("Removing tracks of disc in drive #").Append(String::FromInt(drive)).Append("..."));

	const Array<Track>	*tracks    = BoCA::JobList::Get()->getTrackList.Call();
	Int			 numTracks = tracks->Length();

	for (Int i = numTracks - 1; i >= 0; i--)
	{
		const Track	&track = tracks->GetNth(i);

		if (track.isCDTrack && track.drive == drive)
		{
			BoCA::JobList::Get()->onComponentRemoveTrack.Emit(track);
		}

		SetProgress(1000.0 * (numTracks - i) / numTracks);
	}

	SetText(String("Removed tracks of disc in drive #").Append(String::FromInt(drive)).Append(" from joblist."));

	SetProgress(1000);

	BoCA::JobList::Get()->Unlock();

	return Success();
}
