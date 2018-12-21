ECAD - Edit Schematics and PC Boards as well as manage projects and libraries from other CAD software.

So far, there are no editing features. The work is on creating a good foundation for CAD. Being able to handle objects easaly and having the UI in place comes first. VS2017 is required. The older compilers are not capable of building the parsers. MS is always behind the C++ language evolution.

Code of interest:
XMLSettingsStore derived from CSettingsStore and the use of pugixml. Find that in HEWinApp.cpp. I don't care for using the registry to store so much information. It should go in the appddata of the user. This will also make it easier to change profiles around.

pugixml wrapper:
In xml.h. Previously I had been using a crusty and bloated vesrsion of an XML wrapper around TinyXML. Tiny was hiden in that implamentation. It as worked fine for me for many years, but it was time to move on. The objective of this new pugi wrapper is to keep all of pugi exposed. But to make its use as a application database much easier. The use is already sprinkled about this app. It started with the XMLSettingsStore.

