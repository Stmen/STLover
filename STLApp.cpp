/*  STLover - Simple STL Viewer
 *  Copyright (C) 2020 Gerasim Troeglazov <3dEyes@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.

 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "STLApp.h"
#include "STLWindow.h"

STLoverApplication::STLoverApplication() : BApplication(APP_SIGNATURE),
	fWindowStack(NULL)
{
	InstallMimeType();
}

STLWindow*
STLoverApplication::NewSTLWindow(void)
{
	STLWindow *stlWindow = new STLWindow();

	if (fWindowStack == NULL)
		fWindowStack = new BWindowStack(stlWindow);
	else
		fWindowStack->AddWindow(stlWindow);

	return stlWindow;
}

void
STLoverApplication::RefsReceived(BMessage* msg)
{
	STLWindow *stlWindow = NewSTLWindow();
	stlWindow->PostMessage(msg);
}

void
STLoverApplication::ReadyToRun()
{
	if (CountWindows() == 0)
		NewSTLWindow();
}

void
STLoverApplication::InstallMimeType(void)
{
	BMimeType mime(STL_SIGNATURE);
	status_t ret = mime.InitCheck();
	if (ret != B_OK)
		return;

	ret = mime.Install();
	if (ret != B_OK && ret != B_FILE_EXISTS)
		return;

	mime.SetShortDescription("STL file");
	mime.SetLongDescription("A file format native to the STereoLithography CAD software");

	BMessage message('extn');
	message.AddString("extensions", "stl");
	mime.SetFileExtensions(&message);

	BResources* resources = AppResources();
	if (resources != NULL) {
		size_t size;
		const void* iconData = resources->LoadResource(B_VECTOR_ICON_TYPE, "BEOS:" STL_SIGNATURE,
			&size);
		if (iconData != NULL && size > 0) {
			mime.SetIcon(reinterpret_cast<const uint8*>(iconData), size);
		}
	}
}

BBitmap *
STLoverApplication::GetIcon(const char *iconName, int iconSize)
{
	if (iconName == NULL) {
		app_info inf;
		be_app->GetAppInfo(&inf);

		BFile file(&inf.ref, B_READ_ONLY);
		BAppFileInfo appMime(&file);
		if (appMime.InitCheck() != B_OK)
			return NULL;
	
		BBitmap* icon = new BBitmap(BRect(0, 0, iconSize - 1, iconSize -1), B_RGBA32);
		if (appMime.GetIcon(icon, (icon_size)iconSize) == B_OK)
			return icon;

		delete icon;
		return NULL;
	} else {
		BResources* resources = AppResources();
		if (resources != NULL) {
			size_t size;
			const void* iconData = resources->LoadResource(B_VECTOR_ICON_TYPE, iconName, &size);
			if (iconData != NULL && size > 0) {
				BBitmap* bitmap = new BBitmap(BRect(0, 0, iconSize - 1, iconSize - 1), B_RGBA32);
				status_t status = BIconUtils::GetVectorIcon((uint8*)iconData, size, bitmap);
				if (status == B_OK)
					return bitmap;
			}
		}
		return NULL;
	}
}
