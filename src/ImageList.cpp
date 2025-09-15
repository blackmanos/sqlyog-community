/* Copyright (C) 2013 Webyog Inc

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307, USA

*/


#include "ImageList.h"
#ifdef _WIN32
#include <windows.h>
#include <commctrl.h>
#include "Verify.h"
#else
#include <QIcon>
#endif

#ifdef _WIN32

ImageList::ImageList()
{
    m_himagelist = ::ImageList_Create(16, 16, ILC_MASK | ILC_COLOR32, 1, 1);
    VERIFY(m_himagelist);
    m_imagecount = 0;
}

ImageList::ImageList(wyInt32 width, wyInt32 height, wyUInt32 flags, wyInt32 initialsize, wyInt32 growsize)
{
    m_himagelist = ::ImageList_Create(width, height, flags, initialsize, growsize);
    VERIFY(m_himagelist);
    m_imagecount = 0;
}

ImageList::~ImageList()
{
  ImageList_Destroy(m_himagelist);
}

wyBool
ImageList::Add(HINSTANCE hinstance, wyUInt32 imageid)
{
    HICON hicon;

    hicon = LoadIcon(hinstance, MAKEINTRESOURCE(imageid));
    VERIFY(hicon);

    if(::ImageList_AddIcon(m_himagelist, hicon) == -1)
        return wyFalse;

    VERIFY(::DestroyIcon(hicon));

    m_imagecount++;

    return wyTrue;
}

HIMAGELIST
ImageList::GetImageHandle()
{
    return m_himagelist;
}

#else

ImageList::ImageList()
{
}

ImageList::ImageList(wyInt32 width, wyInt32 height, wyUInt32 flags, wyInt32 initialsize, wyInt32 growsize)
{
    Q_UNUSED(width);
    Q_UNUSED(height);
    Q_UNUSED(flags);
    Q_UNUSED(initialsize);
    Q_UNUSED(growsize);
}

ImageList::~ImageList()
{
}

bool
ImageList::Add(const QIcon& icon)
{
    m_icons.append(icon);
    return true;
}

QList<QIcon>
ImageList::GetImageHandle()
{
    return m_icons;
}

#endif
