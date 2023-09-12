; This file contains two functions: vectorMAC and vectorShift. 
; The first computes the dot product of two vectors.
; The second shifts the elements of a vector down by one position.
; James D. Broesch.
; Last updated: March 3rd, 2013

; vectorMAC(vector1, vector2, size of the vectors, scale factor)
; This function computes the dot product of two vectors.
; W0 holds the address of the signal vector.
; W1 holds the address of the coefficients.
; W2 holds the length of the vectors.
; W3 holds the number of bits used to scale the result.
; The function returns the dot product of the two vectors.
.text ; Place this code in the program space.
.global _vectorMAC ; Make the address globally available.
_vectorMAC: ; Define the starting address of the function.

; Save the status of the control register.
push CORCON
; Save registers W8 and W10 (Only W8 through W10 must be preserved.)
push w8
push w10
; Set saturation mode for ACCA.
bset CORCON,#7
; Use 40 bit mode (super saturation).
bset CORCON,#4
; Set fractional mode.
bclr CORCON,#0

;Free up W0 for the return value.
; Move the x pointer to W8. 
mov W0, W8
; Move the y pointer to w10.
mov W1,W10

; Clear accumulator A and prefetch the first operands.
; Accumulator A is cleared. The first elements of each vector 
;    are loaded into working registers W4 and W5 in preparation
;    for the MAC.
; Specifically:
; Accumulator A is cleared.
; The contents of the address pointed to by W8 are moved to W4.
;    W8 is incremented by 2.
; The contents of the address pointed to by W10 are moved to W5. 
;    W10 is incremented by 2.
clr A,[W8]+=2,w4,[W10]+=2,w5

; Calculate the dot product.
dec W2,W2 ; The repeat count = vector size - 1;
repeat W2 ; For (W2 = 0; W2 < repeat count; W2++)
mac W4 * W5, A,[W8]+=2,W4,[W10]+=2,W5
; W4 and W5 are multiplied together in fractional form.
; The result is added to accumulator A.
; W4 is loaded with the next element of the first vector.
; W5 is loaded with the next element of the second vector.
; The pointer to the first vector is incremented.
; The pointer to the second vector is incremented.
; All this in a single cycle!

;Shift ACCA right the specified number of bits 
; (thus scaling the result).
sftac A,w3

;Return the upper 16 bits of ACCA.
mov ACCAH, w0

;Restore working registers that require it.
pop w10
pop w8
pop CORCON
return


; vectorShift(vector,vector size)
; This function shifts a vector down by one sample.
; W0 contains the address of the vector to shift.
; W1 contains the length of the vector.
; The function does not return a value.
.text
.global _vectorShift
_vectorShift:

;W5 points to the last element in the vector.
dec W1,W1		; Adjust the element count.
rlnc W1,W4 		; Multiply the byte count by two to get the word count.
add W0,W4,W5	; Calculate the last element of the vector.

;W4 points to the next-to-the-last element in the vector;
sub W5,#2,W4

;Adjust the repetition count.
dec W1,W1

;Shift all of the elements down one location.
repeat W1
mov [W4--],[W5--]

return
