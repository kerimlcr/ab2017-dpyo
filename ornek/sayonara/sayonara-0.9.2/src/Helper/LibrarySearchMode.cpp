#include "LibrarySearchMode.h"
#include <QRegExp>
#include <QStringList>

static QStringList no_diacritic_chars;
static QString diacritic_chars;

QString LibraryHelper::convert_search_string(const QString& str, LibraryHelper::SearchModeMask mode)
{
    QString ret = str;
    if(mode & LibraryHelper::CaseInsensitve)
    {
		ret = str.toLower();
    }

    if(mode & LibraryHelper::NoSpecialChars)
    {
		QString special_chars =
				QString::fromUtf8("\\.|'|\"|&|!|\\$|\\+|\\*|\\s|/|\\(|\\)|\\=|-|_|;|:|,|\\?|<|>|\\[|\\]|\\{|\\}|@|€");

		ret.remove(QRegExp(special_chars));
    }

    if(mode & LibraryHelper::NoDiacriticChars)
    {
		if (diacritic_chars.isEmpty()) {
			diacritic_chars = QString::fromUtf8("ŠŒŽšœžŸ¥µÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖØÙÚÛÜÝßàáâãäåæçèéêëìíîïðñòóôõöøùúûüýÿ");
			no_diacritic_chars << "S"<<"OE"<<"Z"<<"s"<<"oe"<<"z"<<"Y"<<"Y"<<"u"<<"A"<<"A"<<"A"<<"A"<<"A"<<"A"<<"AE"<<"C"<<"E"<<"E"<<"E"<<"E"<<"I"<<"I"<<"I"<<"I"<<"D"<<"N"<<"O"<<"O"<<"O"<<"O"<<"O"<<"O"<<"U"<<"U"<<"U"<<"U"<<"Y"<<"s"<<"a"<<"a"<<"a"<<"a"<<"a"<<"a"<<"ae"<<"c"<<"e"<<"e"<<"e"<<"e"<<"i"<<"i"<<"i"<<"i"<<"o"<<"n"<<"o"<<"o"<<"o"<<"o"<<"o"<<"o"<<"u"<<"u"<<"u"<<"u"<<"y"<<"y";
	}

	QString output;
	for (int i = 0; i < ret.length(); i++) {
	    QChar c = ret[i];
	    int found_index = diacritic_chars.indexOf(c);

	    if (found_index < 0) {
		output.append(c);
	    }
	    else {
		QString replacement = no_diacritic_chars[found_index];
		if(mode & LibraryHelper::CaseInsensitve)
		{
		    replacement = replacement.toLower();
		}

		output.append(replacement);
	    }
	}

	ret = output;
    }

    return ret;
}

