/* File phụ của bài 04 — Makefile build chung với 04_static_extern.c */
int g_shared = 100;              /* external linkage: file khác dùng được qua extern */
static int g_hidden = 7;         /* internal linkage: chỉ thấy trong file này */

int helper_get_hidden(void)
{
    return g_hidden;
}
