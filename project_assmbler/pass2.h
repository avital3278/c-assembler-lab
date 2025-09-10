/* pass2.h (ANSI C90) */
#ifndef PASS2_H
#define PASS2_H

#include "data_strct.h"

/* ARE bits (2 LSBs) */
#define ARE_ABSOLUTE	0  /* 00 */
#define ARE_EXTERNAL	1  /* 01 */
#define ARE_RELOCATABLE	2  /* 10 */

/* ריצה של מעבר שני + יצוא קבצים.
   base_name: שם בסיסי לקבצי הפלט (ללא סיומת).
   code:      מערך המילים ממעבר ראשון (קוד+נתונים ברצף).
   code_size: מספר כל המילים (IC+DC-100).
   labels:    טבלת תוויות ממעבר ראשון (עם DATA מוזז).
   entries:   רשימת .entry שנאספה במעבר ראשון.
   externs:   רשימת .extern שנאספה במעבר ראשון.
   IC_final, DC_final: ערכי IC ו-DC בסוף מעבר ראשון.
   error_flag: יסומן אם נמצאו שגיאות (למשל תווית לא מוגדרת).
   הערה: קובץ .ob נכתב עם כל 10 הסיביות כולל ARE.
   מחזיר 1 בהצלחה, 0 אם היו שגיאות.
*/
int pass2_resolve_and_write(
	const char *base_name,
	CodeLine *code, int code_size,
	const LabelEntry *labels, int label_count,
	const EntryExternRef *entries, int entry_count,
	const EntryExternRef *externs, int extern_count,
	int IC_final, int DC_final,
	int *error_flag
);

#endif /* PASS2_H */

