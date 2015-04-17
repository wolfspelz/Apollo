package com.zweitgeist.tools.langman.data;

import java.util.HashMap;

/**
 * A DictionaryEntry is a DictionaryItem that contains a key/value pair and additional information. Every translated String/Text is a DictionaryEntry.
 * @author jkuehne
 *
 */
public class DictionaryEntry extends DictionaryItem{
	
	private String						sKey;
	private String						sComment;	
	private String						sPart;
	private int							nLimit;
	private int							nLineNumber;
	private boolean						bHasChanged;
	private HashMap<Language, String>	hmVals;
	private DefaultDictionary			parent;
	private DictionaryEntry				reference;
	private String						sReference;
	
	
	public DictionaryEntry(
			String sKey,
			HashMap<Language, String> hmVals,
			String sComment,
			String sPart,
			int nLimit,
			boolean bHasChanged,
			int nLineNumber,
			DefaultDictionary parent,
			DictionaryEntry reference,
			String sReference){
		this.sKey = sKey;
		this.hmVals = hmVals;
		this.sComment = sComment;
		this.sPart = sPart;
		this.nLimit = nLimit;
		this.bHasChanged = bHasChanged;
		this.nLineNumber = nLineNumber;
		this.parent = parent;
		this.reference = reference;
		this.sReference = sReference;
	}
	
	
	public DictionaryEntry(String sPart, String sKey) {
		this(sKey, null, null, sPart, 0, false, 0, null, null, null);
	}
	
	
	
	public boolean isDictionaryEntry(){
		return true;
	}
	
	
	
	public String getKey() {		
		return sKey;
	}
	
	
	public void setKey( String sKey ) {
		this.sKey = sKey;
	}
	
	
	
	private String getDefaultValue(Language lang) {
		String sResult = null;
		if (this.getReference() != null){
			sResult = this.getReference().getDefaultValue(lang);
			
		} else {			
			Language defaultLang = null;
			if (lang != null) {
				defaultLang = lang.getDefaultLang();
			}
			while (defaultLang != null){
				sResult = this.hmVals.get(defaultLang);
				if (sResult == null) {				
					defaultLang = defaultLang.getDefaultLang();
				} else {
					break;
				}
			}					
		}
		return sResult;
	}
	
	
	
	public String getValue(Language lang, boolean bUseDefaultIfNotFound){
		String sResult = "";
		if (this.getReference() != null){
			sResult = this.getReference().getValue(lang, bUseDefaultIfNotFound);
		} else {
			sResult = this.hmVals.get(lang);
			if (sResult == null ){
				
				if (bUseDefaultIfNotFound){
					sResult = getDefaultValue(lang);
					if (sResult == null){
						sResult = this.hmVals.get(parent.getDefaultLang());
					}
					if (sResult == null){
						sResult = getDefaultValue(parent.getDefaultLang());
					}
					
					if (sResult == null){				
						parent.callBackKeyWithoutValUsed(this);
					} else {
						parent.callBackDefaultReplacementUsed(this);
					}
					
				} else {
					parent.callBackKeyWithoutValUsed(this);
				}			
			}
		}
		return sResult;
	}
	
	
	
	public void setValue(Language lang, String sValue) {
//		if (this.getReference() == null){
//			this.hmVals.put(lang, sValue);
//		} else {
//			this.getReference().setValue(lang, sValue);			
//		}
		this.hmVals.put(lang, sValue);
	}
	
	
	
	public int getLineNumber() {
		return nLineNumber;
	}


	public void setLineNumber( int nLineNumber ) {
		this.nLineNumber = nLineNumber;
	}


	public String toString(){
		return this.getPart()+"::"+this.getKey();
	}
	
	
	public boolean equals(Object o){		
		DictionaryItem item = (DictionaryItem)o;
		if(!item.isDictionaryEntry()){
			return false;
		}
		DictionaryEntry tempItem = (DictionaryEntry)o;
		boolean bResult = true;
		if(this.getKey() == null){
			bResult &= tempItem.getKey() == null;
		} else {
			if (tempItem.getKey() == null){
				return false;
			}
			bResult &= tempItem.getKey().equals(this.getKey());
		}
		if (bResult){
			if (this.getPart() == null){
				bResult &= tempItem.getPart() == null;
			} else {
				if (tempItem.getPart() == null){
					return false;
				}
				bResult &= tempItem.getPart().equals(this.getPart());
			}
		}
		return bResult;
	}


	public String getComment() {
		return this.sComment;
	}


	public void setComment(String sComment) {
		if (this.getReference() != null){
			this.getReference().setComment(sComment);
		} else {
			this.sComment = sComment;
		}
	}


	public int getLimit() {
		if (this.getReference() != null){
			return this.getReference().getLimit();
		}
		return this.nLimit;
	}


	public void setLimit(int nLimit) {
		if (this.getReference() != null){
			this.getReference().setLimit(nLimit);
		} else {
			this.nLimit = nLimit;
		}
	}


	public String getPart() {
		return sPart;
	}


	public void setPart(String sPart) {
		this.sPart = sPart;
	}


	public void setHasChanged(boolean bHasChanged){
		this.bHasChanged = bHasChanged;
	}
	
	
	public boolean getHasChanged(){
		return this.bHasChanged;
	}
	
	
	public DictionaryEntry getReference() {
		return this.reference;
	}
	
	
	public void setReference(DictionaryEntry reference) {
		this.reference = reference;
	}
	
	
	public String getSReference() {
		return this.sReference;
	}
}
