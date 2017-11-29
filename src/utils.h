/**
 * File is part of project IFJ2017.
 *
 * Brno University of Technology, Faculty of Information Technology
 *
 * @package IFJ2017
 * @authors xomach00 - Martin Omacht, xchova19 - Zdeněk Chovanec, xhendr03 - Petr Hendrych
 */

#ifndef IFJ17_COMPILER_UTILS_H
#define IFJ17_COMPILER_UTILS_H

/**
 * Generates unique identifier
 * @return identifier, NULL on error
 */
char* generate_uid();

/**
 * Concatenate two strings into a new one
 * @param str1 first string
 * @param str2 second string
 * @return new string with concatenated strings, NULL on error
 */
char* concat(const char* str1, const char* str2);

#endif //IFJ17_COMPILER_UTILS_H
