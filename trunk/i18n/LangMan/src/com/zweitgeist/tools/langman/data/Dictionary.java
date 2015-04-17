package com.zweitgeist.tools.langman.data;

import java.io.File;

/**
 * Interface that defines all methods a "Dictionary" has to implement to work with the LangMan. The underlying datastructure
 * may vary - the default implementation (DefaultDictionary) works with a csv file, but other implementation (e.G. DB based) can
 * be implemented. The implementation Dictionary creates the DictionaryItems from the underlying DataStructure.
 * @author jkuehne
 *
 */

public interface Dictionary {
	/** StatisticsType used by getStatistics(): Get all entries that used a key for a translation and found no val */
	public final static int	ST_KEYS_WITHOUT_VALS	= 0;
	/** StatisticsType used by getStatistics(): Get all entries that used the default language and not the desired one for a translateion */
	public final static int	ST_DEFAULT_REPLACEMENTS	= 1;
	
	/**
	 * Returns all Dictionary entries that belong to a given part
	 * @param sPart	The part to get the entries from
	 * @return All entries belonging to sPart
	 */
	public DictionaryEntry[] getPart(String sPart);
	
	/**
	 * Returns all parts that are known to this Dictionary
	 * @return All parts that are known to this Dictionary
	 */
	public String[] getParts();
	
	/**
	 * Returns a language of this Dictionary identified by the short name
	 * @return A language of this Dictionary identified by the short name
	 */
	public Language getLanguage(String sShortName);
	
	/**
	 * Returns all languages that are known to this Dictionary
	 * @return All languages that are known to this Dictionary
	 */
	public Language[] getLanguages();
	
	/**
	 * Checks whether a given part is a part of this Dictionary
	 * @param sPart The part that that is looked for
	 * @return True if sPart is a part 
	 */
	
	public boolean containsPart(String sPart);
	/**
	 * Checks whether a given language is part of this Dictionary
	 * @param lang The language that is looked for
	 * @return True if lang is a part of this Dictionary
	 */
	public boolean contains(Language lang);
	
	
	public void resetStatistics();
	public DictionaryEntry[] getStatistics(int nIndex);
	public Language getDefaultLang();
	public boolean loadPatch(File file, boolean bHideImportWarnings);
	public DictionaryEntry getEntry(String sPart, String sKey);
	public DictionaryEntry[] getEntries();
	public boolean exportToCsv(File file);
	public boolean generateTranslatorFile(File file, Language target, Language[] sources, String[] parts);
	
	/**
	 * Called by DictionaryEntries in case they used the default language for a translation because
	 * the desired language was not available.
	 * @param entry The entry that used the default language
	 */
	public void callBackDefaultReplacementUsed(DictionaryEntry entry);
	/**
	 * Called by DictionaryEntries in case they used no key for a translation because
	 * the desired language was not available and the default language was not available as well or 
	 * should not be used.
	 * @param entry The entry that used the default language
	 */
	public void callBackKeyWithoutValUsed(DictionaryEntry entry);

}
