| **Instruction** | **Operation** | **Description** | **Cycles** |
   | :-- | :-: | :-- | :-: |
   | `add Rd, Rr` | Rd=Rd + Rr | Add without Carry | 1 |
   | `andi Rd, K` | Rd = Rd and K | Logical AND between register Rd and 8-bit constant K | 1 |
   | `bld Rd, b` | Rd(b)	←	T | Bit load from T to Register | 1 |
   | `bst Rd, b` | T	←	Rr(b) | Bit Store from Register to T | 1 |
   | `com Rd` | Rd	←	0xFF - Rd | One’s Complement | 1 |
   | `eor Rd, Rr` | Rd	←	Rd ⊕ Rr | Exclusive OR | 1 |
   | `mul Rd, Rr` | R1:R0	←	Rd x Rr (UU) | Multiply Unsigned | 2 |
   | `pop Rd` | Rd	←	STACK | Pop Register from Stack | 2 |
   | `push Rr` | STACK	←	Rr | Push Register on Stack | 2 |
   | `ret` | PC	←	STACK | 	Subroutine Return | 4/5 |
   | `rol Rd` | ... | Rotate Left Through Carry | 1 |
   | `ror Rd` | ...  | 	Rotate Right Through Carry | 1  |
