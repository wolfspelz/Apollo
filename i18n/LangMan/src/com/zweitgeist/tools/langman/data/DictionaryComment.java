package com.zweitgeist.tools.langman.data;

/**
 * A DictionaryComment is a DictionaryItem that does not contain any key/value pairs. It is used for comments (as the name says :-)) and
 * empty lines, so that these can be preserved on export.
 * @author jkuehne
 *
 */
public class DictionaryComment extends DictionaryItem {
	
	private int 		nLineNo;
	private boolean 	bIsDuplicateKey;
	private String[] 	saContent;
	
	public boolean isDictionaryComment(){
		return true;
	}
	
	
	
	public DictionaryComment(int nLineNo, String[] saContent) {
		super();
		this.nLineNo = nLineNo;
		this.saContent = saContent;
	}



	public int getLineNo() {
		return nLineNo;
	}
	
	
	
	public void setLineNo(int nLineNo) {
		this.nLineNo = nLineNo;
	}
	
	
	
	public String[] getContent() {
		return saContent;
	}
	
	
	
	public void setContent(String[] saContent) {
		this.saContent = saContent;
	}
	
	
	
	public boolean isDuplicateKey(){
		return this.bIsDuplicateKey;
	}
	
	
	
	public void setIsDuplicateKey(boolean bIsDuplicatKey){
		this.bIsDuplicateKey = bIsDuplicatKey;
	}
	
	
	public boolean equals(Object o){
		DictionaryItem item = (DictionaryItem)o;
		if(!item.isDictionaryComment()){
			return false;
		}
		DictionaryComment comment = (DictionaryComment)o;
		return comment.getContent().equals(this.getContent());
	}

}
