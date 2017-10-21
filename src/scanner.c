 /**
 * File is part of project IFJ2017.
 *
 * Brno University of Technology, Faculty of Information Technology
 *
 * @package IFJ2017
 * @authors xomach00 - Martin Omacht, xchova19 - Zdeněk Chovanec, xhendr03 - Petr Hendrych
 */
#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <ctype.h>
#include <string.h>

#include "scanner.h"
#include "fsm.h"
#include "buffer.h"

#define READ_CHAR() getc(scanner->stream);
#define STR_IS(keyword) strcmp(str, keyword) == 0
#define APPEND_TO_BUFFER(ch) buffer_append_c(scanner->buffer, tolower(ch));


Scanner* scanner_init() {
	Scanner* scanner = (Scanner*) malloc(sizeof(Scanner));
	if (scanner == NULL)
		return NULL;

	scanner->buffer = buffer_init(BUFFER_CHUNK);
	if (scanner->buffer == NULL) {
		free(scanner);
		return NULL;
	}

	scanner->stream = stdin;
	scanner->symtable = NULL;
	return scanner;
}

void scanner_free(Scanner* scanner) {
	assert(scanner != NULL);
	buffer_free(scanner->buffer);
	free(scanner);
}

void scanner_set_symtable(Scanner* scanner, HashTable* symtable) {
	assert(scanner != NULL);
	scanner->symtable = symtable;
}

static token_e get_string_token(const char* str) {
	assert(str != NULL);
	size_t len = strlen(str);

	if (len > 1) {
		if (str[0] == 'a') {
			if (STR_IS("as")) 			return TOKEN_KW_AS;
			if (STR_IS("asc"))			return TOKEN_KW_ASC;
			if (STR_IS("and"))			return TOKEN_KW_AND;
		} else if (str[0] == 'd') {
			if (STR_IS("do"))			return TOKEN_KW_DO;
			if (STR_IS("dim"))			return TOKEN_KW_DIM;
			if (STR_IS("declare"))		return TOKEN_KW_DECLARE;
			if (STR_IS("double"))		return TOKEN_KW_DOUBLE;
		} else if (str[0] == 'c') {
			if (STR_IS("chr")) 			return TOKEN_KW_CHR;
			if (STR_IS("continue")) 	return TOKEN_KW_CONTINUE;
		} else if (str[0] == 'e') {
			if (STR_IS("else"))			return TOKEN_KW_ELSE;
			if (STR_IS("end"))			return TOKEN_KW_END;
			if (STR_IS("elseif")) 		return TOKEN_KW_ELSEIF;
			if (STR_IS("exit")) 		return TOKEN_KW_EXIT;
		} else if (str[0] == 'f') {
			if (STR_IS("function")) 	return TOKEN_KW_FUNCTION;
			if (STR_IS("false")) 		return TOKEN_KW_FALSE;
			if (STR_IS("for")) 			return TOKEN_KW_FOR;
		} else if (str[0] == 'i') {
			if (STR_IS("if")) 			return TOKEN_KW_IF;
			if (STR_IS("input")) 		return TOKEN_KW_INPUT;
			if (STR_IS("integer")) 		return TOKEN_KW_INTEGER;
		} else if (str[0] == 'l') {
			if (STR_IS("length")) 		return TOKEN_KW_LENGTH;
			if (STR_IS("loop")) 		return TOKEN_KW_LOOP;
		} else if (str[0] == 'n') {
			if (STR_IS("next")) 		return TOKEN_KW_NEXT;
			if (STR_IS("not")) 			return TOKEN_KW_NOT;
		} else if (str[0] == 's') {
			if (STR_IS("scope")) 		return TOKEN_KW_SCOPE;
			if (STR_IS("string"))		return TOKEN_KW_STRING;
			if (STR_IS("substr")) 		return TOKEN_KW_SUBSTR;
			if (STR_IS("shared")) 		return TOKEN_KW_SHARED;
			if (STR_IS("static")) 		return TOKEN_KW_STATIC;
		} else if (str[0] == 't') {
			if (STR_IS("then")) 		return TOKEN_KW_THEN;
			if (STR_IS("true")) 		return TOKEN_KW_TRUE;
		} else if (STR_IS("while")) {
			return TOKEN_KW_WHILE;
		} else if (STR_IS("or")) {
			return TOKEN_KW_OR;
		} else if (STR_IS("print")) {
			return TOKEN_KW_PRINT;
		} else if (STR_IS("return")) {
			return TOKEN_KW_RETURN;
		} else if (STR_IS("boolean")) {
			return TOKEN_KW_BOOLEAN;
		}
	}

	return TOKEN_IDENTIFIER;
}

token_t* scanner_get_token(Scanner* scanner) {
	assert(scanner != NULL);
	assert(scanner->stream != NULL);

	buffer_clear(scanner->buffer);

	int ch;
	token_t* token = (token_t*) malloc(sizeof(token_t));
	token->attr = NULL;

	FSM {
		STATE(s) {
			ch = READ_CHAR();

			if (ch != '\n' && isspace(ch)) {
				NEXT_STATE(s);
			}

			if (ch == '\n') {
				NEXT_STATE(EOL);
			}

			if (ch == ';') {
				NEXT_STATE(semicolon);
			}

			if (ch == ',') {
				NEXT_STATE(comma);
			}

			if (ch == '=') {
				NEXT_STATE(equal);
			}

			if (ch == '(') {
				NEXT_STATE(left_par);
			}

			if (ch == ')') {
				NEXT_STATE(right_par);
			}

			if (ch == '/') {
				NEXT_STATE(div_real);
			}

			if (ch == '\\') {
				NEXT_STATE(div_int);
			}

			if (ch == '*') {
				NEXT_STATE(mul);
			}

			if (ch == '+') {
				NEXT_STATE(add);
			}

			if (ch == '-') {
				NEXT_STATE(sub);
			}

			if (ch == '<') {
				NEXT_STATE(lt);
			}

			if (ch == '>') {
				NEXT_STATE(gt);
			}

			if (('a' <= ch && ch <= 'z') || ('A' <= ch	&& ch <= 'Z') || ch == '_') {
				APPEND_TO_BUFFER(ch);
				NEXT_STATE(identifier);
			}

			if ('0' <= ch && ch <= '9') {
				APPEND_TO_BUFFER(ch);
				NEXT_STATE(integer);
			}

			if (ch == '!') {
				NEXT_STATE(string_start);
			}

			if (ch == '\'') {
				NEXT_STATE(line_comment);
			}

			if (ch == '&') {
				NEXT_STATE(base);
			}

			if (ch == EOF) {
				NEXT_STATE(end_of_file);
			}

			token->id = LEX_ERROR;
			return token;
		}

		STATE(semicolon) {
			// No need to read character, this is an end state
			token->id = TOKEN_SEMICOLON;
			return token;
		}

		STATE(EOL) {
			token->id = TOKEN_EOL;
			return token;
		}

		STATE(comma) {
			token->id = TOKEN_COMMA;
			return token;
		}

		STATE(equal) {
			token->id = TOKEN_EQUAL;
			return token;
		}

		STATE(left_par) {
			token->id = TOKEN_LPAR;
			return token;
		}

		STATE(right_par) {
			token->id = TOKEN_RPAR;
			return token;
		}

		STATE(div_real) {
			ch = READ_CHAR();
			if (ch == '\'') {
				NEXT_STATE(block_comment);
			}
			else if (ch == '=') {
				token->id = TOKEN_DIVR_ASIGN;
				return token;
			}
			else {
				ungetc(ch, scanner->stream);
				token->id = TOKEN_DIVR;
				return token;
			}
		}

		STATE(block_comment) {
			ch = READ_CHAR();
			if (ch == '\'') {
				NEXT_STATE(block_comment_end);
			}
			else if (ch != EOF) {
				NEXT_STATE(block_comment);
			}
			else {
				ungetc(ch, scanner->stream);
				token->id = LEX_ERROR;
				return token;
			}
		}

		STATE(block_comment_end) {
			ch = READ_CHAR();
			if (ch == '/') {
				NEXT_STATE(s);
			}
			else if (ch != EOF) {
				NEXT_STATE(block_comment);
			}
			else {
				ungetc(ch, scanner->stream);
				token->id = LEX_ERROR;
				return token;
			}
		}

		STATE(div_int) {
			ch = READ_CHAR();
			if (ch == '=') {
				token->id = TOKEN_DIVI_ASIGN;
				return token;
			}
			else {
				ungetc(ch, scanner->stream);
				token->id = TOKEN_DIVI;
				return token;
			}
		}

		STATE(mul) {
			ch = READ_CHAR();
			if (ch == '=') {
				token->id = TOKEN_MUL_ASIGN;
				return token;
			}
			else {
				ungetc(ch, scanner->stream);
				token->id = TOKEN_MUL;
				return token;
			}
		}

		STATE(add) {
			ch = READ_CHAR();
			if (ch == '=') {
				token->id = TOKEN_ADD_ASIGN;
			}
			else {
				ungetc(ch, scanner->stream);
				token->id = TOKEN_ADD;
				return token;
			}
		}

		STATE(sub) {
			ch = READ_CHAR();
			if (ch == '=') {
				token->id = TOKEN_SUB_ASIGN;
				return token;
			}
			else {
				ungetc(ch, scanner->stream);
				token->id = TOKEN_SUB;
				return token;
			}
		}

		STATE(lt) {
			ch = READ_CHAR();
			if (ch == '=') {
				NEXT_STATE(le);
			}
			else if (ch == '>') {
				NEXT_STATE(ne);
			}
			else {
				ungetc(ch, scanner->stream);
				token->id = TOKEN_LT;
				return token;
			}
		}

		STATE(ne) {
			token->id = TOKEN_NE;
			return token;
		}

		STATE(le) {
			token->id = TOKEN_LE;
			return token;
		}

		STATE(gt) {
			ch = READ_CHAR();
			if (ch == '=') {
				NEXT_STATE(ge);
			}
			else {
				ungetc(ch, scanner->stream);
				token->id = TOKEN_GT;
				return token;
			}
		}

		STATE(ge) {
			token->id = TOKEN_GE;
			return token;
		}

		STATE(identifier) {
			ch = READ_CHAR();
			if (('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || ('0' <= ch && ch <= '9') || ch == '_') {
				APPEND_TO_BUFFER(ch);
				NEXT_STATE(identifier);
			}
			else {
				ungetc(ch, scanner->stream);
				token->id = get_string_token(scanner->buffer->arr);
				return token;
			}
		}

		STATE(integer) {
			ch = READ_CHAR();
			if ('0' <= ch && ch <= '9') {
				APPEND_TO_BUFFER(ch);
				NEXT_STATE(integer);
			}
			else if (ch == '.') {
				APPEND_TO_BUFFER(ch);
				NEXT_STATE(fraction);
			}
			else if (ch == 'e' || ch == 'E') {
				APPEND_TO_BUFFER(ch);
				NEXT_STATE(exponent);
			}
			else {
				ungetc(ch, scanner->stream);
				token->id = TOKEN_INT;
				return token;
			}
		}

		STATE(exponent) {
			ch = READ_CHAR();
			if (ch == '+' || ch == '-') {
				APPEND_TO_BUFFER(ch);
				NEXT_STATE(sign);
			}
			else if ('0' <= ch && ch <= '9') {
				APPEND_TO_BUFFER(ch);
				NEXT_STATE(real);
			}
			else {
				ungetc(ch, scanner->stream);
				token->id = LEX_ERROR;
				return token;
			}
		}

		STATE(sign) {
			ch = READ_CHAR();
			if ('0' <= ch && ch <= '9') {
				APPEND_TO_BUFFER(ch);
				NEXT_STATE(real);
			}
			else {
				ungetc(ch, scanner->stream);
				token->id = LEX_ERROR;
				return token;
			}
		}

		STATE(fraction) {
			ch = READ_CHAR();
			if ('0' <= ch && ch <= '9') {
				APPEND_TO_BUFFER(ch);
				NEXT_STATE(real);
			}
			else {
				ungetc(ch, scanner->stream);
				token->id = LEX_ERROR;
				return token;
			}
		}

		STATE(real) {
			ch = READ_CHAR();
			if (ch == 'e' || ch == 'E') {
				APPEND_TO_BUFFER(ch);
				NEXT_STATE(exponent);
			}
			else if ('0' <= ch && ch <= '9') {
				APPEND_TO_BUFFER(ch);
				NEXT_STATE(real);
			}
			else {
				ungetc(ch, scanner->stream);
				token->id = TOKEN_REAL;
				return token;
			}
		}

		STATE(string_start) {
			ch = READ_CHAR();
			if (ch == '"') {
				NEXT_STATE(string);
			}
			else {
				ungetc(ch, scanner->stream);
				token->id = LEX_ERROR;
				return token;
			}
		}

		STATE(string) {
			ch = READ_CHAR();
			if (ch == '\\') {
				APPEND_TO_BUFFER(ch);
				NEXT_STATE(escape_seq);
			} else if (ch == '"') {
				NEXT_STATE(string_end);
			} else if (ch != EOF && ch != '\n') {
				APPEND_TO_BUFFER(ch);
				NEXT_STATE(string);
			} else {
				ungetc(ch, scanner->stream);
				token->id = LEX_ERROR;
				return token;
			}
		}

		STATE(escape_seq) {
			ch = READ_CHAR();
			if ('0' <= ch && ch <= '2') {
				APPEND_TO_BUFFER(ch);
				NEXT_STATE(esc_num_one);
			}
			else if (ch != EOF && ch != '\n') {
				APPEND_TO_BUFFER(ch);
				NEXT_STATE(string);
			}
			else {
				ungetc(ch, scanner->stream);
				token->id = LEX_ERROR;
				return token;
			}
		}

		STATE(esc_num_one) {
			ch = READ_CHAR();
			if ('0' <= ch && ch <= '5') {
				APPEND_TO_BUFFER(ch);
				NEXT_STATE(esc_num_two);
			}
			else {
				ungetc(ch, scanner->stream);
				token->id = LEX_ERROR;
				return token;
			}
		}

		STATE(esc_num_two) {
			ch = READ_CHAR();
			if ('0' <= ch && ch <= '5') {
				APPEND_TO_BUFFER(ch);
				NEXT_STATE(string);
			}
			else {
				ungetc(ch, scanner->stream);
				token->id = LEX_ERROR;
				return token;
			}
		}

		STATE(string_end) {
			token->id = TOKEN_STRING;
			return token;
		}

		STATE(line_comment) {
			ch = READ_CHAR();
			if (ch == '\n') {
				token->id = TOKEN_EOL;
				return token;
			}
			else if (ch == EOF) {
				NEXT_STATE(end_of_file);
			}
			else {
				NEXT_STATE(line_comment);
			}
		}

		STATE(end_of_file) {
			token->id = TOKEN_EOF;
			return token;
		}

		STATE(base) {
			ch =  READ_CHAR();
			if (ch == 'b' || ch == 'B') {
				NEXT_STATE(binary);
			}
			else if (ch == 'o' || ch == 'O') {
				NEXT_STATE(octal);
			}
			else if (ch == 'h' || ch == 'H') {
				NEXT_STATE(hexa);
			}
			else {
				ungetc(ch, scanner->stream);
				token->id = LEX_ERROR;
				return token;
			}
		}

		STATE(binary) {
			ch = READ_CHAR();
			if (ch == '0' || ch == '1') {
				APPEND_TO_BUFFER(ch);
				NEXT_STATE(int_bin);
			}
			else {
				ungetc(ch, scanner->stream);
				token->id = LEX_ERROR;
				return token;
			}
		}

		STATE(octal) {
			ch = READ_CHAR();
			if ('0' <= ch && ch <= '7') {
				APPEND_TO_BUFFER(ch);
				NEXT_STATE(int_octal);
			}
			else {
				ungetc(ch, scanner->stream);
				token->id = LEX_ERROR;
				return token;
			}
		}

		STATE(hexa) {
			ch = READ_CHAR();
			if (('0' <= ch && ch <= '9') || ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z')) {
				APPEND_TO_BUFFER(ch);
				NEXT_STATE(int_hexa);
			}
			else {
				ungetc(ch, scanner->stream);
				token->id = LEX_ERROR;
				return token;
			}
		}

		STATE(int_bin) {
			ch = READ_CHAR();
			if (ch == '0' || ch == '1') {
				APPEND_TO_BUFFER(ch);
				NEXT_STATE(int_bin);
			}
			else {
				ungetc(ch, scanner->stream);
				token->id = TOKEN_INT;
				return token;
			}
		}

		STATE(int_octal) {
			ch = READ_CHAR();
			if ('0' <= ch && ch <= '7') {
				APPEND_TO_BUFFER(ch);
				NEXT_STATE(int_octal);
			}
			else {
				ungetc(ch, scanner->stream);
				token->id = TOKEN_INT;
				return token;
			}
		}

		STATE(int_hexa) {
			ch = READ_CHAR();
			if (('0' <= ch && ch <= '9') || ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z')) {
				APPEND_TO_BUFFER(ch);
				NEXT_STATE(int_hexa);
			}
			else {
				ungetc(ch, scanner->stream);
				token->id = TOKEN_INT;
				return token;
			}
		}
	}

	free(token);
	return NULL;
}

