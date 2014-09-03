#pragma once

#include <string>
#include <stack>
#include <string>
#include <sstream>
#include <assert.h>

class XmlWriter
{
private:
	std::ostream *os;

	int identLevel;
	bool isElementBracketOpen;

	std::stack<std::string> openElements;

	std::string Ident()
	{
		std::string ident = "";

		for (int i = 0; i < identLevel; i++)
			ident += "\t";

		return ident;
	}

	void CloseElementBracket()
	{
		*os << ">\n";

		isElementBracketOpen = false;
	}

public:
	XmlWriter(std::ostream *os, int identLevel)
	{
		this ->os = os;
		this ->identLevel = identLevel;

		isElementBracketOpen = false;
	}

	template <typename ValType>
	void CreateElement(const char *name, ValType data)
	{
		*os << Ident() << "<" << name << ">" << data << "</" << name << ">\n";
	}

	template <typename ValType>
	void CreateElement(const char *name, const char *attribName, ValType attribValue)
	{
		OpenElement(name);
		WriteAttribute(attribName, attribValue);

		*os << " />\n";
		identLevel--;
		isElementBracketOpen = false;
		openElements.pop();
	}

	template <typename A1Type, typename A2Type>
	void CreateElement(const char *name, const char *att1Name, A1Type att1Val, const char *att2Name, A2Type att2Val)
	{
		OpenElement(name);
		WriteAttribute(att1Name, att1Val);
		WriteAttribute(att2Name, att2Val);

		*os << " />\n";
		identLevel--;
		isElementBracketOpen = false;
		openElements.pop();
	}

	void OpenElement(const char *name)
	{
		if (isElementBracketOpen)
			CloseElementBracket();

		*os << Ident() << "<" << name;

		openElements.push(name);
		isElementBracketOpen = true;
		identLevel++;
	}

	void CloseElement()
	{
		if (isElementBracketOpen)
			CloseElementBracket();

		identLevel--;

		*os << Ident() << "</" << openElements.top() << ">\n";

		openElements.pop();
	}

	template <typename ValType>
	void WriteAttribute(const char *name, const ValType value)
	{
		assert(isElementBracketOpen == true);

		*os << " " << name << "=\"" << value << "\"";
	}

	template <typename ValType>
	void WriteElementCdata(const ValType data)
	{
		if (isElementBracketOpen)
			CloseElementBracket();

		*os << Ident() << "<![CDATA[" << data << "]]>\n";
	}

	template <typename ValType>
	void WriteElementData(const ValType data)
	{
		if (isElementBracketOpen)
			CloseElementBracket();

		*os << Ident() << data << "\n";
	}
};
