Name:           girl
Version:        9.8.0
Release:        1%{?dist}
Summary:        GNOME Internet Radio Locator program
License:        GPLv2+
URL:            http://people.gnome.org/~ole/girl
Group:          Applications/Internet
Source:         http://people.gnome.org/~ole/%{name}/%{name}-%{version}.tar.xz

BuildRequires:  gtk2-devel
BuildRequires:  libgnome-devel
BuildRequires:  libxml2-devel
BuildRequires:  gnome-vfs2-devel
BuildRequires:  libgnomeui-devel
BuildRequires:  intltool
BuildRequires:  itstool
BuildRequires:  libappstream-glib
BuildRequires:  desktop-file-utils
BuildRequires:  gstreamer-devel
BuildRequires:  gstreamer1-plugins-bad-free-devel
BuildRequires:  gstreamer1-plugins-base-devel
Requires:       gstreamer1 >= 1.8.3
Requires:       gstreamer1-plugins-ugly >= 1.8.3

%description
GNOME Internet Radio Locator (GIRL) is a program that allows the user
to easily locate live radio programs by broadcasters on the Internet.

GIRL is developed on the GNOME platform and it requires gstreamer for
playback.

Enjoy Internet Radio.

%prep
%setup -q

%build
%configure --with-recording --disable-silent-rules

%install
%make_install
%find_lang %{name} --with-man

%check
appstream-util validate-relax --nonet %{buildroot}/%{_datadir}/appdata/%{name}.appdata.xml
desktop-file-validate %{buildroot}/%{_datadir}/applications/%{name}.desktop

%post
/bin/touch --no-create %{_datadir}/icons/hicolor &>/dev/null || :
/usr/bin/update-desktop-database &> /dev/null || :

%postun
if [ $1 -eq 0 ] ; then
    /bin/touch --no-create %{_datadir}/icons/hicolor &>/dev/null
    /usr/bin/gtk-update-icon-cache %{_datadir}/icons/hicolor &>/dev/null || :
fi
/usr/bin/update-desktop-database &> /dev/null || :

%posttrans
/usr/bin/gtk-update-icon-cache %{_datadir}/icons/hicolor &>/dev/null || :

%files -f %{name}.lang
%doc AUTHORS DEBIAN LETTER MAILINGLIST NEWS README TODO VERSION YP-DIRS ChangeLog
%license COPYING
%{_bindir}/%{name}
%{_datadir}/%{name}/
%{_datadir}/appdata/%{name}.appdata.xml
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/hicolor/*/apps/%{name}.png
%{_mandir}/man1/%{name}.1*
%{_datadir}/help/*/%{name}

%changelog
* Sun Feb 05 2017 Ole Aamot - 9.8.0-1.fc25
- Girl 9.8.0 build on Fedora Linux 25

* Sat Feb 04 2017 Ole Aamot - 9.7.2-1.fc25
- Girl 9.7.2 build on Fedora Linux 25

* Wed Jan 18 2017 Ole Aamot - 9.7.1-1.fc25
- Girl 9.7.1 build on Fedora Linux 25

* Tue Jan 17 2017 Ole Aamot - 9.7.0-1.fc24
- Girl 9.7.0 build on Fedora Linux 24

* Sat Jan 07 2017 Ole Aamot - 9.6.2-1.fc24
- Girl 9.6.2 build on Fedora Linux 24

* Sat Dec 10 2016 Ole Aamot - 9.6.1-1.fc24
- Girl 9.6.1 build on Fedora Linux 24

- Girl 9.6.0 build on Fedora Linux 24
- Remove dependency on totem
- Add dependency on gstreamer1

* Sat Sep 24 2016 Ole Aamot - 9.5.3-1.fc23
- Girl 9.5.3 build on Fedora Linux 23

* Fri Sep 23 2016 Ole Aamot - 9.5.2-1.fc23
- Girl 9.5.2 build on Fedora Linux 23

* Sat Aug 27 2016 Ole Aamot - 9.5.1-1.fc23
- Girl 9.5.1 build on Fedora Linux 23

* Sun Aug 14 2016 Ole Aamot - 9.5.0-1.fc23
- Girl 9.5.0 build on Fedora Linux 23

* Thu Jul 07 2016 Ole Aamot - 9.4.0-1.fc23
- Girl 9.4.0 build on Fedora Linux 23

* Sat Jun 25 2016 Ole Aamot - 9.3.0-1.fc23
- Girl 9.3.0 build on Fedora Linux 23

* Sat Jun 11 2016 Ole Aamot - 9.2.0-1.fc23
- Girl 9.2.0 build on Fedora Linux 23

* Tue Jun 07 2016 Ole Aamot - 9.1.0-1.fc23
- Girl 9.1.0 build on Fedora Linux 23

* Sun May 22 2016 Ole Aamot - 9.0.0-1.fc23
- Girl 9.0.0 build on Fedora Linux 23

* Sat May 21 2016 Ole Aamot - 8.5.0-1.fc23
- Girl 8.5.0 build on Fedora Linux 23

* Sun May 01 2016 Ole Aamot - 8,4.2-1.fc23
- Girl 8.4.2 build on Fedora Linux 23

* Wed Apr 27 2016 Ole Aamot - 8.4.1-1.fc23
- Girl 8.4.1 build on Fedora Linux 23

* Tue Mar 08 2016 Ole Aamot - 8.4.0-1.fc23
- Girl 8.4.0 build on Fedora Linux 23

* Tue Mar 08 2016 Ole Aamot - 8.3.16-1.fc23
- Girl 8.3.16 build on Fedora Linux 23

* Sun Jan 17 2016 Ole Aamot - 8.0.1-1.fc23
- Cleanup
- Rebuild

* Sat Jan 16 2016 Ole Aamot - 8.0.0-1.fc23
- Girl 8.0.0 build on Fedora Linux 23

* Sun Jan 03 2016 Ole Aamot - 7.0.0-1.fc23
- Girl 7.0.0 build on Fedora Linux 23

* Sat Aug 08 2015 Ole Aamot - 6.1.0-1.fc21
- Girl 6.1.0 build on Fedora Linux 21

* Sat May 30 2015 Ole Aamot - 6.0.0-1.fc21
- Girl 6.0.0 build on Fedora Linux 21

* Fri May 08 2015 Ole Aamot - 5.0.0-1.fc21
- Girl 5.0.0 build on Fedora Linux 21

* Sun May 03 2015 Ole Aamot - 4.0.0-1.fc21
- Girl 4.0.0 build on Fedora Linux 21

* Wed Apr 29 2015 Ole Aamot - 3.2.0-1.fc21
- Girl 3.2.0 build on Fedora Linux 21

* Wed Apr 29 2015 Ole Aamot - 3.1.0-1.fc21
- Girl 3.1.0 build on Fedora Linux 21

* Sat Apr 25 2015 Ole Aamot - 3.0.0-1.fc21
- Girl 3.0.0 build on Fedora Linux 21

* Tue Mar 24 2015 Ole Aamot - 2.0.0-1.fc21
- Girl 2.0.0 build on Fedora Linux 21

* Sat Mar 21 2015 Ole Aamot - 1.9.0-1.fc21
- Girl 1.9.0 build on Fedora Linux 21

* Sat Mar 14 2015 Ole Aamot - 1.8.0-1.fc21
- Girl 1.8.0 build on Fedora Linux 21

* Sun Mar 08 2015 Ole Aamot - 1.7.0-1.fc21
- Girl 1.7.0 build on Fedora Linux 21

* Sat Mar 07 2015 Ole Aamot - 1.6.0-1.fc21
- Girl 1.6.0 build on Fedora Linux 21

* Sun Mar 01 2015 Ole Aamot - 1.5.1-1.fc21
- Cleanup spec file with help from Martin Gansser

* Sat Feb 28 2015 Ole Aamot - 1.5.0-1.fc21
- Girl 1.5.0 build on Fedora Linux 21

* Sat Feb 21 2015 Ole Aamot - 1.4.0-1.fc21
- Girl 1.4.0 build on Fedora Linux 21

* Sat Feb 14 2015 Ole Aamot - 1.3.0-1.fc21
- Girl 1.3.0 build on Fedora Linux 21

* Fri Jan 23 2015 Ole Aamot - 1.2.0-1.fc21
- Girl 1.2.0 build on Fedora Linux 21

* Sun Jan 18 2015 Ole Aamot - 1.1.1-1.fc21
- Girl 1.1.1 build on Fedora Linux 21

* Sat Jan 17 2015 Ole Aamot - 1.1.0-1
- Girl 1.1.0 build on Fedora Linux 21

* Thu Jan 01 2015 Ole Aamot - 1.0.0-1
- Girl 1.0.0 build on Fedora Linux 21

* Sat Dec 27 2014 Ole Aamot - 0.9.0-1
- Girl 0.9.0 build on Fedora Linux 21

* Sat Dec 20 2014 Ole Aamot - 0.8.0-1
- Girl 0.8.0 build on Fedora Linux 21

* Sun Dec 14 2014 Ole Aamot - 0.7.3-1
- Girl 0.7.3 build on Fedora Linux 21

* Sun Dec 14 2014 Ole Aamot - 0.7.2-1
- Girl 0.7.2 build on Fedora Linux 21

* Sat Dec 13 2014 Ole Aamot - 0.7.1-1
- Girl 0.7.1 build on Fedora Linux 21

* Fri Dec 12 2014 Ole Aamot - 0.7.0-1
- Girl 0.7.0 build on Fedora Linux 21

* Sun Dec 07 2014 Ole Aamot - 0.6.2-1
- Girl 0.6.2 build on Fedora Linux 21

* Sat Dec 06 2014 Ole Aamot - 0.6.1-1
- Girl 0.6.1 build on Fedora Linux 21

* Tue Dec 02 2014 Ole Aamot - 0.6.0-1
- Girl 0.6.0 build on Fedora Linux 21

* Sat Nov 29 2014 Ole Aamot - 0.5.2-1
- Girl 0.5.2 build on Fedora Linux 21

* Sat Nov 29 2014 Ole Aamot - 0.5.1-1
- Girl 0.5.1 build on Fedora Linux 21

* Sat Nov 29 2014 Ole Aamot - 0.5.0-1
- Girl 0.5.0 build on Fedora Linux 21

* Sat Nov 22 2014 Ole Aamot - 0.4.0-1
- Girl 0.4.0 build on Fedora Linux 20

* Sat Nov 15 2014 Ole Aamot - 0.3.0
- Modify description of Girl

* Sat Nov  8 2014 Ole Aamot - 0.2.0
- Add dependency on 'totem'.

* Sat Nov  1 2014 Ole Aamot - 0.1.0
- Initial Girl build on Fedora Core 20
