/* fith compiler */
/* PUBLIC DOMAIN */


// first thought strategy is to search for end of object
// however this binary pattern can show up in integers and doubles
// therefore we must lex past them if they come up
// also must count objects to avoid nesting trap
// we dont need to deal with strings because they are utf-8 WRONG
// utf-8 has these tokens in them, crap we do have to deal with strings
static void lex_skipObj(const u8 **YYCURSORx)
{
	const u8 *YYCURSOR = *YYCURSORx;
	u32 obj_count=0;

loop: // label for looping within the lexxer

	/*!re2c                          // start of re2c block **/
	re2c:define:YYCTYPE = "u8";      //   configuration that defines YYCTYPE
	re2c:yyfill:enable  = 0;         //   configuration that turns off YYFILL
									 //
	* { 
		goto loop;
	}//   default rule with its semantic action skip garbage

	[\x86] { // ION Object
		obj_count++;
		goto loop;
	}

	[\x87] { // ION Object End
		if(obj_count==0){
			*YYCURSORx = YYCURSOR;
			return;
		}
		obj_count--;
		goto loop;
	}

	[\x8a] { // ION float
		YYCURSOR+=8;
		goto loop;
	}

	[\x8b-\x92] { // ION Ints
		YYCURSOR+=((*(YYCURSOR-1))-0x8A);
		goto loop;
	}

	[\x93-\xbe] { // ION Strings
		YYCURSOR+=((*(YYCURSOR-1))-0x92);
		goto loop;
	}

	[\xbf] { // ION unknown string length
		YYCURSOR+=strlen((const char *)YYCURSOR+44)+1+44;
		goto loop;
	}

	*/                               // end of re2c block
}

// same strat as objects explained above
static void lex_skipArray(const u8 **YYCURSORx)
{
	const u8 *YYCURSOR = *YYCURSORx;
	u32 array_count=0;

loop: // label for looping within the lexxer

	/*!re2c                          // start of re2c block **/
	re2c:define:YYCTYPE = "u8";      //   configuration that defines YYCTYPE
	re2c:yyfill:enable  = 0;         //   configuration that turns off YYFILL
									 //
	* { 
		goto loop;
	}//   default rule with its semantic action skip garbage

	[\x88] { // ION Array
		array_count++;
		goto loop;
	}

	[\x89] { // ION Array End
		if(array_count==0){
			*YYCURSORx = YYCURSOR;
			return;
		}
		array_count--;
		goto loop;
	}

	[\x8a] { // ION float
		YYCURSOR+=8;
		goto loop;
	}

	[\x8b-\x92] { // ION Ints
		YYCURSOR+=((*(YYCURSOR-1))-0x8A);
		goto loop;
	}
	
	[\x93-\xbe] { // ION Strings
		YYCURSOR+=((*(YYCURSOR-1))-0x92);
		goto loop;
	}

	[\xbf] { // ION unknown string length
		YYCURSOR+=strlen((const char *)YYCURSOR+44)+1+44;
		goto loop;
	}

	*/                               // end of re2c block
}

static void lex_skipVal(const u8 **YYCURSORx)
{
	const u8 *YYCURSOR = *YYCURSORx;

loop: // label for looping within the lexxer

	/*!re2c                          // start of re2c block **/
	re2c:define:YYCTYPE = "u8";      //   configuration that defines YYCTYPE
	re2c:yyfill:enable  = 0;         //   configuration that turns off YYFILL
									 //
	* { 
		*YYCURSORx=0;
		printf("invalid ION inside lex_skipVal\n");
		return;
	}//   default rule with its semantic action skip garbage

	[\x80] { // ION 0
		*YYCURSORx = YYCURSOR;
		return;
	}

	[\x81] { // ION 1
		*YYCURSORx = YYCURSOR;
		return;
	}

	[\x82] { // ION negative sign
		goto loop;
	}

	[\x83] { // ION true
		*YYCURSORx = YYCURSOR;
		return;
	}

	[\x84] { // ION false
		*YYCURSORx = YYCURSOR;
		return;
	}

	[\x85] { // ION null
		*YYCURSORx = YYCURSOR;
		return;
	}

	[\x86] { // ION Object
		lex_skipObj(&YYCURSOR);
		*YYCURSORx = YYCURSOR;
		return;
	}

	[\x88] { // ION Array
		lex_skipArray(&YYCURSOR);
		*YYCURSORx = YYCURSOR;
		return;
	}

	[\x8a] { // ION float
		YYCURSOR+=8;
		*YYCURSORx = YYCURSOR;
		return;
	}

	[\x8b-\x92] { // ION Ints
		YYCURSOR+=((*(YYCURSOR-1))-0x8A);
		*YYCURSORx = YYCURSOR;
		return;
	}

	[\x93-\xbe] { // ION Strings
		YYCURSOR+=((*(YYCURSOR-1))-0x92);
		*YYCURSORx = YYCURSOR;
		return;
	}

	[\xbf] { // ION unknown string length
		YYCURSOR+=strlen((const char *)YYCURSOR+44)+1+44;
		*YYCURSORx = YYCURSOR;
		return;
	}

	*/                               // end of re2c block
}

static Data lex_returnVal(const u8 *YYCURSORx)
{
	const u8 *YYCURSOR = YYCURSORx;
	const u8 *cursor;
	u8 *buff;
	u64 length;
	s64 num;
	Data retVal;
	s32 neg=0;

loop: // label for looping within the lexxer

	/*!re2c                          // start of re2c block **/
	re2c:define:YYCTYPE = "u8";      //   configuration that defines YYCTYPE
	re2c:yyfill:enable  = 0;         //   configuration that turns off YYFILL
									 //
	* { 

		printf("invalid ION inside lex_returnVal\n");
		retVal.i=0;
		return retVal;
	}//   default rule with its semantic action skip garbage

	[\x80] { // ION 0
		retVal.i=0;
		return retVal;
	}

	[\x81] { // ION 1
		retVal.i=(1-neg);
		return retVal;
	}

	[\x82] { // ION negative sign
		neg=2;
		goto loop;
	}

	[\x83] { // ION true
		retVal.s = (u8 *)ION_TRUE_VAL;
		return retVal;
	}

	[\x84] { // ION false
		retVal.s = (u8 *)ION_FALSE_VAL;
		return retVal;
	}

	[\x85] { // ION null
		retVal.s = (u8 *)ION_NULL_VAL;
		return retVal;
	}

	[\x86] { // ION Object
		cursor = YYCURSOR-1;
		lex_skipObj(&YYCURSOR);
		length = YYCURSOR-cursor+4; // get length
		//length +=3;
		//length /=4;
		buff = malloc(length);
		buff[0] = ((length&0x00000000FF000000)>>24)|0x80;
		buff[1] = (length&0x0000000000FF0000)>>16;
		buff[2] = (length&0x000000000000FF00)>>8;
		buff[3] = length&0xFF;
		memcpy(&buff[4], cursor, (YYCURSOR-cursor+4));
		retVal.s = buff;
		return retVal;
	}

	[\x88] { // ION Array
		cursor = YYCURSOR-1;
		lex_skipArray(&YYCURSOR);
		length = YYCURSOR-cursor+4; // get length
		//length +=3;
		//length /=4;
		buff = malloc(length);
		buff[0] = ((length&0x00000000FF000000)>>24)|0x80;
		buff[1] = (length&0x0000000000FF0000)>>16;
		buff[2] = (length&0x000000000000FF00)>>8;
		buff[3] = length&0xFF;
		memcpy(&buff[4], cursor, (YYCURSOR-cursor+4));
		retVal.s = buff;
		return retVal;
	}

	[\x8a] { // ION float
		retVal.d = ION_toDouble(YYCURSOR);
		return retVal;
	}

	[\x8b-\x92] { // ION Ints
		num = ION_Int(YYCURSOR, ((*(YYCURSOR-1))-0x8A));
		num*=(1-neg);
		retVal.i=num;
		return retVal;
	}

	[\x93-\xbe] { // ION Strings
		length =((*(YYCURSOR-1))-0x92);
		buff = malloc(length+1);
		memcpy(buff, YYCURSOR, length);
		buff[length]=0;
		retVal.s = buff;
		return retVal;
	}

	[\xbf] { // ION unknown string length
		length=strlen((const char *)YYCURSOR+44)+1+44;
		buff = malloc(length);
		memcpy(buff, YYCURSOR, length);
		retVal.s = buff;
		return retVal;
	}

	*/                               // end of re2c block
}

static u32 getPathLen(const u8 *path)
{
	u32 path_len=0;
	// find length of this part of the path
	while ( (path[path_len]!=0) && (path[path_len]!='[') && (path[path_len]!='.') )
	{
		path_len++;
	}
	return path_len;
}

// 3 cases: error, matching key-value found, and nothing found, returns end
// given an ION object, search for given path, ending with '.' '[' or '\0'
static s32 lex_searchObj(const u8 **inputx, u8 *pathx, u32 path_len)
{                                    //
	const u8 *YYCURSOR = *inputx;
	u8 *path = pathx;
	u32 key_len;
	u32 i=0, isNullTerminated=0;
	
	while(1){
		key_len = (*YYCURSOR) - ION_STRING01 +1;
		if (key_len>45)
		{
			printf("Error: %d,%d  lex_searchObj Keys must be strings\n", key_len,*YYCURSOR);
			exit(1);
		}
		YYCURSOR++; // point at key string
		if ((path_len>44)&&(key_len==ION_STRING_N))
		{
			key_len = strlen((const char *)YYCURSOR+44)+44;
			isNullTerminated=1;
		}
		if(key_len != path_len){
			goto next;
		}
		for (; i<key_len; i++)
		{
			if (YYCURSOR[i] != path[i])
			{
				break;
			}
		}
		if(i==key_len)
		{
			*inputx = &YYCURSOR[i];
			return 0; // success
		}
		i=0;
		
		next:
		YYCURSOR+= key_len+isNullTerminated; // skip past key
		isNullTerminated=0;
		lex_skipVal(&YYCURSOR); // skip value
		if(*YYCURSOR==ION_OBJ_END) // end of object found
		{
			*inputx = YYCURSOR;
			return 1;
		}
	}
}

// given an ION object, search for given path, ending with '.' '[' or '\0'
static void lex_searchArray(const u8 **inputx, u8 **pathx, Context * c)
{                                    //
	const u8 *YYCURSOR = *inputx;
	u8 *path = *pathx;
	u32 target_index=0;
	u32 array_index=0;
	u32 path_len=0;
	
	// find length of this part of the path
	while ( (path[path_len]!=0) && (path[path_len]!='[') && (path[path_len]!='.') )
	{
		path_len++;
	}
	*pathx = &path[path_len]; // move path forward
	// closing bracket ']' is not needed
	path_len--;
	
	//    Arrays are different beasts, there are several cases
	// 1. user input text based number: starts with [0-9]
	//   * parse number, use as array index for search.
	// 2. user input text based var: starts with [$]
	//   * look for variable, use as array index for search.
	// 3. user input append or 'end' macro: starts with [#]
	//   * find end of array, return position of array end
	// 4. user group append: starts with [?]
	//   * same as above (3)
	// 5. user input is blank: []
	//   * use top of stack as index into array, users may hate this if chained?
	
	if (path_len==0)
	{
		// get top of stack
		//printf("lex_searchArraypath_len==0\n");
		DECREMENT_STACK
		target_index = (c->stk-1)->i;
		// use as index
	} else if ( (*path == '?') || (*path == '#') ) {
		// skiparray
		lex_skipArray(&YYCURSOR);
		// back up one
		YYCURSOR--;
		// return
		*inputx = YYCURSOR;
		return; // success
	}
	// we have an index
	
	while(1){
		if(target_index == array_index)
		{
			*inputx = YYCURSOR;
			return; // success
		}
		array_index++;
		lex_skipVal(&YYCURSOR); // skip value
		if(*YYCURSOR==ION_ARR_END) // end of object found
		{
			*inputx = ION_NULL_VAL;
			return; // nothing found
		}
	}

}

/* Context
 * lex_findPath is called AFTER we have verified this is an ION element
 * and have the length (in 4 byte increments) to find the true end search back-
 * wards till you hit obj end or array end, garbage at end set to 0s (max 3)
 * Extract: return value of search path
 * ** this function does most of the work, append and mass add are not useful
 * Set: Set value if path exists, if not insert into obj or try to append to
 * the array if it is one past the last element.
 * ** this function finds location for insert/update
 * Remove: remove key/value pair or array element
 * ** this function finds location for removal
 * Type?: return type of value: Obj, Array, Int, Float, String (maybe others?)
 * Maybe I dont do the whole 4 bytes thing? I can add more bits and get an exact
 * match AND use first part as number of elements. options are
 * byte length 4 bytes, 4 byte aligned zero mutation only alloc exten/contract
 * element length: 30 bits [!3!2...] approx 2 bytes per elem + 2. Max 1 bil len
 * use a different data structure for anything else
 * total length, which if we do the whole 'functional' way should be omitted
 * so this would put us at 8 byte header... not too bad
 * {"color"green} vs {"color":"green"}
 * '.#' could be key for number of elements (length of array)
*/
// given an ION element, search for given path
// refactored
static const u8 * lex_findPath(const u8 *inputx, u8 *pathx, Context * c)
{                                    //
	const u8 *YYCURSOR = inputx;
	u8 *path = pathx;
	u32 path_len;
	/*s32 res;*/
	
	while((*path)){
		if(*path == '.') // we are looking for a key
		{
			if(*YYCURSOR != ION_OBJ_START)
			{
				printf("ERROR lex_findPath: not a ION Obj.\n");
				return 0;
			}
			path++;
			YYCURSOR++;
			path_len = getPathLen(path);
			//printf("lex_searchObj.%s\n", path);
			/*res = */lex_searchObj(&YYCURSOR, path, path_len);
			path+=path_len;
		} else if (*path == '[') {
			if(*YYCURSOR != ION_ARR_START)
			{
				printf("ERROR lex_findPath: not a ION Array.\n");
				return 0;
			}
			path++;
			YYCURSOR++;
			lex_searchArray(&YYCURSOR, &path, c);
		} else {
			printf("ERROR lex_findPath: not a valid path ION Obj.\n");
			return 0;
		}
	}
	return YYCURSOR;
}

static int lex_printFSON(u8 *YYCURSORx)
{                                    //
	u8 *YYCURSOR = YYCURSORx+4;
	u8 *start;
	u32 object_count=0;
	u32 array_count=0;
	u32 flags_index=0;
	u8 flags[512]={0};
	//u32 fson_length;
	//u8 *startMangledString;
	//YYCURSOR = *YYCURSOR_p;
	//startMangledString = YYCURSOR;
	// get length of ION
	//~ fson_length= (((*outx)&0x3F)<<24);
	//~ fson_length+=((*(outx+1))<<16);
	//~ fson_length+=((*(outx+2))<<8);
	//~ fson_length+= (*(outx+3));
	
	if (*YYCURSOR==ION_OBJ_START)
	{
		fputc ('{', stdout);
		object_count++;
		// set object flag
		flags[flags_index] |= 0x01;
	} else if (*YYCURSOR==ION_ARR_START){
		fputc ('[', stdout);
		array_count++;
	} else {
		printf("invalid ION inside lex_printFSON: %X\n", *YYCURSOR);
	}
	YYCURSOR++;
	

loop: // label for looping within the lexxer
	start = YYCURSOR;
	//printf("CURRENT CHAR (%X) \n",*(YYCURSOR));

	/*!re2c                          // start of re2c block **/
	re2c:define:YYCTYPE = "u8";      //   configuration that defines YYCTYPE
	re2c:yyfill:enable  = 0;         //   configuration that turns off YYFILL
									 //
	* { 
		printf("invalid ION inside lex_printFSON: %X %X\n",*(YYCURSOR-2),*(YYCURSOR-1));
		return -1;
	}//   default rule with its semantic action skip garbage

	[\x80] { // ION 0
		fputc ('0', stdout);
		if( ((*YYCURSOR)!=ION_OBJ_END) && ((*YYCURSOR)!=ION_ARR_END) )
		{
			fputc (',', stdout);
			flags[flags_index] &= 0xFD;
		}
		goto loop;
	}

	[\x81] { // ION 1
		fputc ('1', stdout);
		if( ((*YYCURSOR)!=ION_OBJ_END) && ((*YYCURSOR)!=ION_ARR_END) )
		{
			fputc (',', stdout);
			flags[flags_index] &= 0xFD;
		}
		goto loop;
	}

	[\x82] { // ION negative sign
		fputc ('-', stdout);
		if( ((*YYCURSOR)!=ION_OBJ_END) && ((*YYCURSOR)!=ION_ARR_END) )
		{
			fputc (',', stdout);
			flags[flags_index] &= 0xFD;
		}
		goto loop;
	}

	[\x83] { // ION true
		printf("true");
		if( ((*YYCURSOR)!=ION_OBJ_END) && ((*YYCURSOR)!=ION_ARR_END) )
		{
			fputc (',', stdout);
			flags[flags_index] &= 0xFD;
		}
		goto loop;
		
	}

	[\x84] { // ION false
		printf("false");
		if( ((*YYCURSOR)!=ION_OBJ_END) && ((*YYCURSOR)!=ION_ARR_END) )
		{
			fputc (',', stdout);
			flags[flags_index] &= 0xFD;
		}
		goto loop;;
	}

	[\x85] { // ION null
		printf("null");
		if( ((*YYCURSOR)!=ION_OBJ_END) && ((*YYCURSOR)!=ION_ARR_END) )
		{
			fputc (',', stdout);
			flags[flags_index] &= 0xFD;
		}
		goto loop;
	}

	[\x86] { // ION Object
		fputc ('{', stdout);
		object_count++;
		// set object flag
		flags_index++;
		flags[flags_index] |= 0x01;
		goto loop;
	}

	[\x87] { // ION Object End
		fputc ('}', stdout);
		object_count--;
		if ( (object_count==0) && (array_count==0) )
		{
			return 0;
		}
		if( ((*YYCURSOR)!=ION_OBJ_END) && ((*YYCURSOR)!=ION_ARR_END) )
		{
			fputc (',', stdout);
		}
		flags_index--;
		flags[flags_index] &= 0xFD;
		goto loop;
	}

	[\x88] { // ION Array
		fputc ('[', stdout);
		array_count++;
		// clear object flag
		flags_index++;
		flags[flags_index] &= 0xFE;
		goto loop;
	}

	[\x89] { // ION Array End
		fputc (']', stdout);
		array_count--;
		if ( (array_count==0) && (object_count==0) )
		{
			return 0;
		}
		if( ((*YYCURSOR)!=ION_OBJ_END) && ((*YYCURSOR)!=ION_ARR_END) )
		{
			fputc (',', stdout);
		}
		flags_index--;
		flags[flags_index] &= 0xFD;
		goto loop;
	}

	[\x8a] { // ION float
		Data dbl;
		u64 tmp, res;
		start++;
		tmp=*start;
		start++;
		res = tmp<<56;
		tmp=*start;
		start++;
		res += tmp<<48;
		tmp=*start;
		start++;
		res += tmp<<40;
		tmp=*start;
		start++;
		res += tmp<<32;
		tmp=*start;
		start++;
		res += tmp<<24;
		tmp=*start;
		start++;
		res += tmp<<16;
		tmp=*start;
		start++;
		res += tmp<<8;
		tmp=*start;
		start++;
		res += tmp;
		YYCURSOR+=8;
		dbl.i=(s64)res;
		printf("%.3f",dbl.d);
		if( ((*YYCURSOR)!=ION_OBJ_END) && ((*YYCURSOR)!=ION_ARR_END) )
		{
			fputc (',', stdout);
			flags[flags_index] &= 0xFD;
		}
		goto loop;
	}

	[\x8b-\x92] { // ION Ints
		s64 ival;
		ival = ION_Int(YYCURSOR, ((*start)-0x8A));
		printf("%ld",ival);
		YYCURSOR+=((*start)-0x8A);
		if( ((*YYCURSOR)!=ION_OBJ_END) && ((*YYCURSOR)!=ION_ARR_END) )
		{
			fputc (',', stdout);
			flags[flags_index] &= 0xFD;
		}
		goto loop;
	}

	[\x93-\xbe] { // ION Strings
		u8 tmp = *(YYCURSOR+((*start)-0x92));
		*(YYCURSOR+((*start)-0x92))=0;
		fputc ('\"', stdout);
		printf("%s", (const char *)YYCURSOR);
		*(YYCURSOR+((*start)-0x92))=tmp;
		fputc ('\"', stdout);
		YYCURSOR+=((*start)-0x92);
		if( ((*YYCURSOR)!=ION_OBJ_END) && ((*YYCURSOR)!=ION_ARR_END) )
		{
			//printf("(%d)",flags[flags_index]&0x03);
			if((flags[flags_index]&0x03)!=1) // either in array or is a val
			{
				fputc (',', stdout);
				// set value flag
				//flags[flags_index] |= 0x02;
				flags[flags_index] &= 0xFD;
			} else {
				fputc (':', stdout);
				// clear value flag for key
				//flags[flags_index] &= 0xFD;
				flags[flags_index] |= 0x02;
			}
		}
		goto loop;
	}

	[\xbf] { // ION unknown string length
		u32 len = strlen((const char *)YYCURSOR+44)+1+44; // including null
		fputc ('\"', stdout);
		printf("%s", (const char *)YYCURSOR);
		fputc ('\"', stdout);
		YYCURSOR+=len;
		if( ((*YYCURSOR)!=ION_OBJ_END) && ((*YYCURSOR)!=ION_ARR_END) )
		{
			if((flags[flags_index]&0x03)!=1) // either in array or is a val
			{
				fputc (',', stdout);
				// set value flag
				//flags[flags_index] |= 0x02;
				flags[flags_index] &= 0xFD;
			} else {
				fputc (':', stdout);
				// clear value flag for key
				//flags[flags_index] &= 0xFD;
				flags[flags_index] |= 0x02;
			}
		}
		goto loop;
	}

	*/                               // end of re2c block
}

static int lex_FSONToString(const u8 *YYCURSORx, u8 *outx)
{                                    //
	const u8 *YYCURSOR = YYCURSORx;
	u8 *out = outx+4;
	const u8 *start;
	u32 object_count=0;
	u32 array_count=0;
	//u8 *startMangledString;
	//YYCURSOR = *YYCURSOR_p;
	//startMangledString = YYCURSOR;
	

loop: // label for looping within the lexxer
	start = YYCURSOR;

	/*!re2c                          // start of re2c block **/
	re2c:define:YYCTYPE = "u8";      //   configuration that defines YYCTYPE
	re2c:yyfill:enable  = 0;         //   configuration that turns off YYFILL
									 //
	* { 
		printf("invalid ION inside lex_FSONToString\n");
		return -1;
	}//   default rule with its semantic action skip garbage

	[\x80] { // ION 0
		*out = '0';
		out++;
		goto loop;
	}

	[\x81] { // ION 1
		*out = '1';
		out++;
		goto loop;
	}

	[\x82] { // ION negative sign
		*out = '-';
		out++;
		goto loop;
	}

	[\x83] { // ION true
		out = (u8*)stpcpy((char *)out, "true");
		goto loop;
	}

	[\x84] { // ION false
		out = (u8*)stpcpy((char *)out, "false");
		goto loop;;
	}

	[\x85] { // ION null
		out = (u8*)stpcpy((char *)out, "null");
		goto loop;
	}

	[\x86] { // ION Object
		*out = '{';
		out++;
		object_count++;
		goto loop;
	}

	[\x87] { // ION Object End
		*out = '}';
		out++;
		object_count--;
		if ( (object_count==0) && (array_count==0) )
		{
			*out = 0;
			out++;
			return 0;
		}
		goto loop;
	}

	[\x88] { // ION Array
		*out = '[';
		out++;
		array_count++;
		goto loop;
	}

	[\x89] { // ION Array End
		*out = ']';
		out++;
		array_count--;
		if ( (array_count==0) && (object_count==0) )
		{
			*out = 0;
			out++;
			return 0;
		}
		goto loop;
	}

	[\x8a] { // ION float
		Data d;
		u64 tmp, res;
		
		start++;
		tmp=*start;
		start++;
		res = tmp<<56;
		tmp=*start;
		start++;
		res += tmp<<48;
		tmp=*start;
		start++;
		res += tmp<<40;
		tmp=*start;
		start++;
		res += tmp<<32;
		tmp=*start;
		start++;
		res += tmp<<24;
		tmp=*start;
		start++;
		res += tmp<<16;
		tmp=*start;
		start++;
		res += tmp<<8;
		tmp=*start;
		start++;
		res += tmp;
		YYCURSOR+=8;
		d.i=(s64)res;
		out += sprintf((char *)out, "%f",d.d);
		goto loop;
	}

	[\x8b-\x92] { // ION Ints
		s64 ival;
		ival = ION_Int(YYCURSOR, ((*start)-0x8A));
		out += sprintf((char *)out, "%ld",ival);
		YYCURSOR+=((*start)-0x8A);
		goto loop;
	}

	[\x93-\xbe] { // ION Strings
		*out = '\"';
		out++;
		memcpy(out, YYCURSOR, ((*start)-0x92));
		out+=((*start)-0x92);
		*out = '\"';
		out++;
		YYCURSOR+=((*start)-0x92);
		goto loop;
	}

	[\xbf] { // ION unknown string length
		u32 len = strlen((const char *)YYCURSOR+44)+44; // including null
		*out = '\"';
		out++;
		memcpy(out, YYCURSOR, len);
		out+=len;
		*out = '\"';
		out++;
		YYCURSOR+=len+1;
		goto loop;
	}

	*/                               // end of re2c block
}

static int lex_stringToFSON(const u8 *YYCURSORx, u8 *outx)
{                                    //
	const u8 *YYCURSOR = YYCURSORx;
	u8 *out = outx+4;
	const u8 *YYMARKER;
	const u8 *start;
	u64 length;
	//u8 *startMangledString;
	//YYCURSOR = *YYCURSOR_p;
	//startMangledString = YYCURSOR;
	

loop: // label for looping within the lexxer
	start = YYCURSOR;

	/*!re2c                          // start of re2c block **/
	re2c:define:YYCTYPE = "u8";      //   configuration that defines YYCTYPE
	re2c:yyfill:enable  = 0;         //   configuration that turns off YYFILL
									 //
	* { goto loop; }//   default rule with its semantic action skip garbage
	[\x00] { 
		// write in metadata in begining
		length = out - outx; // get length
		//length +=3;
		//length /=4;
		*outx = ((length&0x00000000FF000000)>>24)|0x80;
		outx++;
		*outx = (length&0x0000000000FF0000)>>16;
		outx++;
		*outx = (length&0x000000000000FF00)>>8;
		outx++;
		*outx = length&0xFF;
		return 1;
	}             // EOF rule with null sentinal
	
	[ \n\t\r] { // JSON Whitespace
		goto loop;
	}

	"{" { // JSON Object start
		*out = ION_OBJ_START;
		out++;
		goto loop;
	}

	"}" { //  JSON Object end
		*out = ION_OBJ_END;
		out++;
		goto loop;
	}

	"[" { // JSON Whitespace
		*out = ION_ARR_START;
		out++;
		goto loop;
	}

	"]" { // JSON Whitespace
		*out = ION_ARR_END;
		out++;
		goto loop;
	}
	
	"," { // JSON Whitespace
		goto loop;
	}
	
	":" { // JSON Whitespace
		goto loop;
	}

	"true" { // JSON Whitespace
		*out = ION_TRUE;
		out++;
		goto loop;
	}

	"false" { // JSON Whitespace
		*out = ION_FALSE;
		out++;
		goto loop;
	}

	"null" { // JSON Whitespace
		*out = ION_NULL;
		out++;
		goto loop;
	}

	"-"?( "0"|([1-9][0-9]*) )  { // JSON integer
		ION_writeInt(&out, atol( (const char *)start ));
		goto loop;
	}

	"-"?( "0"|([1-9][0-9]*) ) ( ("." [0-9]+) |  (("e"|"E") [+-] [0-9]+) | ( ("." [0-9]+) (("e"|"E") [+-] [0-9]+) ) )  { // JSON floating
		//~ Data ionNumber;
		//~ u64 ionInt;
		
		ION_writeFloat(&out, atof( (const char *)start ));
		//~ ionNumber.d = atof( (const char *)start );
		//~ ionInt = (u64)ionNumber.i;
		//~ *out = ION_FLOAT;
		//~ out++;
		//~ *out = (ionInt&0xFF00000000000000)>>56;
		//~ out++;
		//~ *out = (ionInt&0x00FF000000000000)>>48;
		//~ out++;
		//~ *out = (ionInt&0x0000FF0000000000)>>40;
		//~ out++;
		//~ *out = (ionInt&0x000000FF00000000)>>32;
		//~ out++;
		//~ *out = (ionInt&0x00000000FF000000)>>24;
		//~ out++;
		//~ *out = (ionInt&0x0000000000FF0000)>>16;
		//~ out++;
		//~ *out = (ionInt&0x000000000000FF00)>>8;
		//~ out++;
		//~ *out = ionInt&0xFF;
		//~ out++;
		goto loop;
	}

	 ["] ([^"] | ([\\] ["]))* ["] { // JSON string
		u64 length;
		length = YYCURSOR-start-1;
		start++;
		if(length>45){length++;}
		ION_writeString(&out, start, length);
		//~ if (length < 45)
		//~ {
			//~ *out = ION_STRING01+(length-1);
		//~ } else {
			//~ *out = ION_STRING_N;
			//~ out++;
			//~ memcpy(out, start, length);
			//~ out+=length;
			//~ *out =0; //null terminate long strings
			//~ out++;
			//~ goto loop;
		//~ }
		//~ out++;
		//~ memcpy(out, start, length);
		//~ out+=length;
		goto loop;
	}

	*/                               // end of re2c block
}
