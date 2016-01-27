#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include "..\..\src\commonfunc\StkObject.h"

void StkObjectTest()
{
	StkObject* Elem1 = new StkObject(_T("SoftwareCompany"));

	StkObject* Elem2 = new StkObject(_T("Product"));
	Elem2->AppendAttribute(new StkObject(_T("Name"), _T("Xxxxx paint")));
	Elem2->AppendAttribute(new StkObject(_T("Price"), _T("11000")));

	StkObject* Elem3 = new StkObject(_T("Product"));
	Elem3->AppendAttribute(new StkObject(_T("Name"), _T("Aaaaa 3D")));
	Elem3->AppendAttribute(new StkObject(_T("Price"), _T("39000")));

	StkObject* Elem4 = new StkObject(_T("Product"));

	StkObject* Elem5 = new StkObject(_T("License"));
	Elem5->AppendAttribute(new StkObject(_T("Cap"), 322.44f));
	Elem2->AppendChildElement(Elem5);

	for (int Loop = 0; Loop < 20; Loop++) {
		StkObject* Elem6 = new StkObject(_T("Product"));
		Elem6->AppendAttribute(new StkObject(_T("Name"), _T("qwer")));
		Elem6->AppendAttribute(new StkObject(_T("Price"), 10000));
		Elem1->AppendChildElement(Elem6);
	}

	Elem1->AppendAttribute(new StkObject(_T("CompanyName"), _T("TakeuchiSoft")));
	Elem1->AppendAttribute(new StkObject(_T("Employee"), _T("110")));
	Elem1->AppendAttribute(new StkObject(_T("Address"), _T("Shizuoka Pref. Yaizu city")));
	Elem1->AppendChildElement(Elem2);
	Elem1->AppendChildElement(Elem3);
	Elem1->AppendChildElement(Elem4);
	Elem1->ToXml(2);
	delete Elem1;
}
