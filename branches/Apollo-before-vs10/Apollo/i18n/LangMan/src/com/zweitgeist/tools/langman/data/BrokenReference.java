package com.zweitgeist.tools.langman.data;

public class BrokenReference extends DictionaryEntry {

	public BrokenReference(String part, String key) {
		super(part, key);
	}
	
	public String getValue(Language lang, boolean bUseDefaultIfNotFound){
		return null;
	}

}
