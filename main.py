import _roman
import argparse


def main():
    """
    Driver code (main function) for the calculator.
    
    Returns:
    - str: If successful run with parsing a Roman Numeral equation to a single Roman_numeral. 
        The alternative is the error in string form from ValueError or IndexError.
    - int: If only put in one argument
    """
    parser = argparse.ArgumentParser(description="Processes a Roman Numeral equation with (+, -, *, / (), [] ). using PEMDAS")
    
    parser.add_argument('equation', nargs=argparse.REMAINDER, help='The equation that is of cmd line arguments, either inputted as a string "" or as a line of Roman Numerals and operations')

    args = parser.parse_args()
    
    # print("Equation entered is \"%s\"" % "".join(args.equation))

    numeric_equation = _roman.convert("".join(args.equation))
    # print("Converted to \"%s\"" % numeric_equation)

    final_num = _roman.solve(numeric_equation)
    # print("The solved equation = %d" % final_num)

    print("%s" % _roman.int_to_roman(final_num))
    

if __name__ == '__main__':
    main()
