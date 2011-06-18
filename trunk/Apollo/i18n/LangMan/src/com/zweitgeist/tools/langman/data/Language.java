package com.zweitgeist.tools.langman.data;

/**
 * Represents a language. For each language used in a dictionary one Language Object is created.
 * @author jkuehne
 *
 */
public class Language {
	
	private String			sShortName;
	private String			sName;
	private Language		defaultLang;
	
	
	public Language(String sShortName){
		this.sShortName = sShortName;
	}
	
	
	
	public String getName() {
		return sName;
	}
	
	
	
	public void setName(String sName) {
		this.sName = sName;
	}
	
	
	
	public String getShortName() {
		return sShortName;
	}
	
	
	
	public void setShortName(String sShortName) {
		this.sShortName = sShortName;
	}
	
	
	
	public Language getDefaultLang() {
		return this.defaultLang;
	}
	
	
	
	public void setDefaultLang(Language defaultLang){
		this.defaultLang = defaultLang;
	}
	
	
	
	public String toString(){
		return this.getShortName();
	}
	
	
	
	public int hashCode(){
		int result = 0;
		for (int i=0; i<this.sShortName.length(); i++){
			result += i*this.sShortName.toLowerCase().charAt(i);
		}
		return result;
	}
	
	
	
	public boolean equals(Object o){
		Language oLang = (Language)o;
		boolean result = this.getShortName().equalsIgnoreCase(oLang.getShortName());
		return result;
	}
}
