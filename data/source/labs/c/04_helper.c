/* Helper file for lab 04 — the Makefile builds it together with 04_static_extern.c */
int g_shared = 100;              /* external linkage: other files can use it via extern */
static int g_hidden = 7;         /* internal linkage: visible only in this file */

int helper_get_hidden(void)
{
    return g_hidden;
}
