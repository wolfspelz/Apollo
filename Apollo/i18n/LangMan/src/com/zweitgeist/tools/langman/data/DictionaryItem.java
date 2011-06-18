package com.zweitgeist.tools.langman.data;

/**
 * Every "information item" of a Dictionary is a DictionaryItem - currently it either is a Dictionarycomment or a DictionaryEntry
 * @author jkuehne
 *
 */
public abstract class DictionaryItem {
	
	
	public boolean isDictionaryComment(){
		return false;
	}
	
	
	
	public boolean isDictionaryEntry(){
		return false;
	}

}
