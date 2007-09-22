#ifndef exceptions_h_
#define exceptions_h_

#include <setjmp.h>

#include <dom/core/exceptions.h>

/* Usage:
 	TRY
 		THROW(DOM_NOT_FOUND_ERR);
		THROW_IF_ERR(dom_document_get_doctype(...));
	CATCH(ex)
		printf("exception: %d\n", ex);
	ENDTRY
*/
#define TRY              __exvalue=setjmp(__exbuf); \
                         if (__exvalue==0) {
#define CATCH(x)         } else {                   \
                         int x = __exvalue;
#define ENDTRY           }
#define THROW(x)         longjmp(__exbuf, x)

#define THROW_IF_ERR(x)    \
  do {                     \
    int err = x;           \
    if (err != DOM_NO_ERR) \
      THROW(err);          \
    } while (0)

jmp_buf __exbuf;
int __exvalue;

#endif

