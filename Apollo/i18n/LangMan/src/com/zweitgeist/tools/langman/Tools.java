package com.zweitgeist.tools.langman;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.util.Vector;

/**
 * Contains various static helper methods.
 * @author jkuehne
 *
 */
public class Tools {
	
	public final static int EOF			= -1;
	public final static int BUF_SIZE	= 4096;
	
	/**
	 * Reads a given text file in a specified encoding and returns a String containing the content of
	 * the text file.
	 * @param f			The text file to read
	 * @param sCharset	The encoding the text file was encoded in (eg UTF-8, Shift-JIS, ...). If no
	 * 					no encoding is given (sCharset == null) the default encoding of the machine is used
	 * @return			A String containing the content of f
	 * @throws IOException
	 */
	public static String readTextFile(File f, String sCharset) throws IOException {
		InputStreamReader isr;
		if (sCharset != null){
			isr = new InputStreamReader( new FileInputStream(f), sCharset);
		} else {
			isr = new InputStreamReader(new FileInputStream(f));
		}		
		BufferedReader reader = new BufferedReader(isr) ;
		StringBuffer resultBuffer = new StringBuffer();
		char[] cbuf = new char[BUF_SIZE];
		int read = 0;
		while ((read = reader.read(cbuf)) != EOF) {
			resultBuffer.append(cbuf, 0, read);
		}
		return resultBuffer.toString();
	}
	
	
	
	/**
	 * Writes a String to a text file
	 * @param f			The file to save the String in
	 * @param sText		The String that shall be written
	 * @param sCharset	The encoding that shall be used. If no encoding is given 
	 * 					(sCharset == null) the default encoding of the machine is used. 
	 * @throws IOException
	 */
	public static void writeTextFile(File f, String sText, String sCharset) throws IOException {
		OutputStreamWriter osw;
		if (sCharset != null){
			osw = new OutputStreamWriter(new FileOutputStream(f), sCharset);
		} else {
			osw = new OutputStreamWriter(new FileOutputStream(f));
		}
		osw.write(sText);
		osw.flush();
		osw.close();
	}
	
	
	
	/**
	 * Turns a String into a valid CSV "field". If the String represents a number it is left the way it is. If
	 * it is any other text it will be surrounded by the "-character and all "-characters inside the text will
	 * be escaped as ""
	 * @param sValue The value to be converted into CSV format
	 * @return		 sValue converted to CSV format 
	 */
	public static String encodeCsv(String sValue){
		String result = sValue;
		if (result == null || result.trim().length() == 0){
			return "";
		} else if (!Tools.isInt(result)){
			result = result.replaceAll("\"", "\"\"");
			result = "\""+result+"\"";
		}
		return result;
	}
	
	
	
	/**
	 * Parses a String representing a line of values encoded as CSV
	 * @param sLine	String representing a row of values encoded as CSV
	 * @return Array of Strings containing the values found in sLine
	 */
	public static String[] parseCsv(String sLine){
		return parseCsv(sLine, true);
	}
	
	
	
	/**
	 * Parses a String representing a line of values encoded as CSV
	 * @param sLine	String representing a row of values encoded as CSV
	 * @param bVerbose Flag indicating whether error messages should be printed to the console
	 * @return Array of Strings containing the values found in sLine
	 */
	public static String[] parseCsv(String sLine, boolean bVerbose){
		final int ST_READY_FOR_NEW		= 0;
		final int ST_NUM		 		= 1;
		final int ST_STRING			 	= 2;
		Vector<String> vColumns = new Vector<String>();
		String sCurrentChar = "";
		String sRest = sLine;
		StringBuffer sbCurrentColumn = new StringBuffer();
		int nState = ST_READY_FOR_NEW;
		while(sRest.length() > 0){
			sCurrentChar = sRest.substring(0,1);
			if(sRest.length()>1){
				sRest = sRest.substring(1, sRest.length());
			} else {
				sRest = "";
			}
			
			switch (nState){
				case ST_READY_FOR_NEW:
					if (sCurrentChar.equals(";")){
						vColumns.add("");
					} else if (isInt(sCurrentChar)){
						nState = ST_NUM;
						sbCurrentColumn.append(sCurrentChar);
					} else if (sCurrentChar.equals("\"")){
						nState = ST_STRING;
					} else {
						if (bVerbose){
							System.err.println( "In line '"+sLine+"' I did not expect '"+sCurrentChar+"' after '"+sbCurrentColumn+"' and before '"+sRest);
						}
						return null;
					}
					break;
					
				case ST_NUM:
					if (sCurrentChar.equals(";")){
						vColumns.add(sbCurrentColumn.toString());
						sbCurrentColumn = new StringBuffer();
						nState = ST_READY_FOR_NEW;
					} else if (isInt(sCurrentChar)){
						sbCurrentColumn.append(sCurrentChar);						
					} else {
						if (bVerbose){
							System.err.println( "In line '"+sLine+"' I did not expect '"+sCurrentChar+"' after '"+sbCurrentColumn+"' and before '"+sRest);
						}
						return null;
					}
					break;
					
				case ST_STRING:
					if (sCurrentChar.equals("\"")){
						if (sRest.length() >= 1 && sRest.substring(0,1).equals( "\"")){
							sRest = sRest.substring(1, sRest.length());
							sbCurrentColumn.append("\"");
						} else if (sRest.length() >= 1 && sRest.substring(0,1).equals( ";")){
							sRest = sRest.substring(1, sRest.length());
							sCurrentChar = ";";
							vColumns.add(sbCurrentColumn.toString());
							sbCurrentColumn = new StringBuffer();
							nState = ST_READY_FOR_NEW;
						} else if (sRest.length() == 0 ){
							vColumns.add(sbCurrentColumn.toString());
							nState = ST_READY_FOR_NEW;
						}
					} else {						
						sbCurrentColumn.append(sCurrentChar);
					}
					break;
				}
		}

		if (sCurrentChar.equals(";")){
			vColumns.add("");
		}
		if(nState == ST_NUM){
			vColumns.add(sbCurrentColumn.toString());
		} else if (nState == ST_STRING){
			if (bVerbose){
				System.err.println( "Something went wrong on line '"+sLine+"' ... most likely a string was not properly closed!");
			}
			return null;
		}
		return vColumns.toArray(new String[vColumns.size()]);
	}
	
	
	
	/**
	 * Reads a CSV file and returns a String array containing the unescaped values found in the file. 
	 * @param file		The CSV file to read
	 * @param sCharset	The Encoding of the CSV file 
	 * @return The values found in file
	 */
	public static String[][] readCsv(File file, String sCharset){
		String[][] result = null;
		try {			
			InputStream is = new FileInputStream( file ); 
			BufferedReader ir = new BufferedReader( new InputStreamReader( is, sCharset ) );
			String sLine = "";
			Vector<String[]> vLines = new Vector<String[]>();
			while (sLine != null){
				sLine = ir.readLine();
				if (sLine != null){
					vLines.add(Tools.parseCsv(sLine));
				}							
			}
			result = vLines.toArray(new String[vLines.size()][]);
		} catch (Exception e){
			System.err.println("Exception caught in readCsv: "+e);
		}
		return result;
	}
	
	
	
	/**
	 * Checks whether a String represents a number or not
	 * @param s	The String to check
	 * @return	true if s represents a number, else false
	 */
	public static boolean isInt(String s){
		try{
			new Integer(s);
		} catch (Exception e){
			return false;
		}
		return true;
	}
	
	
	
	/**
	 * Returns a new file that has the same name/path as fIn, but another extension (sNewExtension)
	 * @param fIn				The "original" file
	 * @param sOldExtension		The extension to change
	 * @param sNewExtension		The replacement of the old extension
	 * @return					File with replaced extension. Null if any of the given parameters is null or fIn does not have sOldExtension
	 */
	public static File replaceExtension(File fIn, String sOldExtension, String sNewExtension){
		File fResult = null;
		if (fIn != null && sOldExtension != null && sNewExtension != null){
			String sInName = fIn.getAbsolutePath();
	        if (sInName.endsWith(sOldExtension)){
	        	String sResultName = sInName.substring(0, sInName.length()-3)+sNewExtension;
	        	fResult = new File(sResultName);
	        }
		}
        return fResult;
	}
	
	
	
	/**
	 * Escapes occurances of a String sCharToEscape in a String sString by another String sEscapeChat
	 * @param sString		String to Escape
	 * @param sCharToEscape	Occurances within sString that shall be escaped
	 * @param sEscapeChar	The character to escape with
	 * @return An escaped String
	 */
	public static String escapeString(String sString, String sCharToEscape, String sEscapeChar) {
		String sResult = sString;
		String sCharToEscapeDoubleBackslashed = sCharToEscape.replaceAll("\\\\", "\\\\\\\\");
		String sEscapeCharDoubleBackslashed = sEscapeChar.replaceAll("\\\\", "\\\\\\\\");		
		sResult = sResult.replaceAll(sCharToEscapeDoubleBackslashed, sEscapeCharDoubleBackslashed+sCharToEscapeDoubleBackslashed);
		return sResult;
	}
	
}
