#pragma once
#include "virtual_validator.h"
#include "virtual_board.h"
#include "virtual_tile.h"
#include <QDebug>
using namespace std;
using lambda = function<bool(scoord)>;
using checker = function<bool(scoord, bool&)>;

VirtualValidator::VirtualValidator(VirtualBoard* mother_board) :
	vboard(mother_board),
	valid_moves{},
	check(false),
	has_moved{ false }, // FIX: {} or {{}}?
	rooks_kings{ {0,0}, {4, 0}, {7, 0}, {0, 7}, {4, 7}, {7, 7} },
	castling_destination{ {2, 0}, {-1, -1}, {6, 0}, {2, 7}, {-1, -1}, {6, 7} },
	should_be_free{
		{{3, 0}, {2, 0}, {1, 0}},
		{},
		{{5, 0}, {6, 0}},
		{{3, 7}, {2, 7}, {1, 7}},
		{},
		{{5, 7}, {6, 7}}
	},
	should_be_safe{
		{{4, 0}, {3, 0}, {2, 0}},
		{},
		{{4, 0}, {5, 0}, {6, 0}},
		{{4, 7}, {3, 7}, {2, 7}},
		{},
		{{4, 7}, {5, 7}, {6, 7}}
	},
	perp_dir{ { 0,  1 }, { 0,  -1 }, { 1,  0 },  { -1, 0 } },
	diag_dir{ { 1,  1 }, { 1, -1 },  { -1,  1 }, { -1, -1 } },
	inBoard([](scoord coord) -> bool {
	return coord.x >= 0 && coord.x < 8 && coord.y >= 0 && coord.y < 8;
		}),
	occupied([&](scoord coord) -> bool {
	return theTile(coord).piece_name != 'e';
		}),
	differentColor([&](scoord coord) -> bool {
	// may be realized differently, with either turn or piece color
	return occupied(coord) && theTile(coord).piece_color != theTurn();
		}),
	pieceName([&](scoord coord) -> char {
	return theTile(coord).piece_name;
		}),
	addValid([&](scoord coord) -> void {
	valid_moves.emplace(coord);
		}),
	fastThrough([&](scoord coord, scoord add, checker stop_cond, bool& result) -> void {
	// here range is (from, to] or (from, to), depending on stop_cond
	do { coord.x += add.x, coord.y += add.y; } while (!stop_cond(coord, result));
		}),
	fastLine([&](scoord coord, checker stop_cond, const list<scoord>& adds) -> bool {
	bool result = false;
	for (auto dir : adds) {
		fastThrough(coord, dir, stop_cond, result);
		if (result)
			return true;
	}
	return false;
		}),
	enemyFinder([&](scoord coord, lambda comparison, lambda borders, bool& result) -> bool {
	if (!borders(coord)) { // borders checking should always go first
		result = false; // then it's the end of a cycle
		return true;
	}
	else if (differentColor(coord)) {
		result = comparison(coord);
		return true; // then stop
	}
	else
		return false; // then continue
		})
{}

// beginning of virtual methods
VirtualTile& VirtualValidator::theTile(scoord coord)
{
	return vboard->theTile(coord);
}

bool VirtualValidator::theTurn()
{
	return vboard->turn;
}

VirtualTile& VirtualValidator::theWKing()
{
	return *vboard->white_king;
}

VirtualTile& VirtualValidator::theBKing()
{
	return *vboard->black_king;
}

void VirtualValidator::moveVirtually(scoord from, scoord to, vove& saved_move)
{
	saved_move = { theTile(from), theTile(to) };
	vboard->moveNormally(vboard->theTile(from), vboard->theTile(to));
}

void VirtualValidator::revertVirtualMove(vove& saved_move)
{
	vboard->revertMoveNormally(saved_move);
}

const std::vector<halfmove>& VirtualValidator::theStory()
{
	return vboard->history;
}
// end of virtual methods

void VirtualValidator::kingPotential(scoord coord, list<scoord>& coords)
{
	int x = coord.x, y = coord.y;
	coords = { { x - 1, y + 1 }, { x, y + 1 }, { x + 1, y + 1 },
			   { x - 1, y },                   { x + 1, y },
			   { x - 1, y - 1 }, { x, y - 1 }, { x + 1, y - 1 } };
}

void VirtualValidator::knightPotential(scoord coord, list<scoord>& coords)
{
	int x = coord.x, y = coord.y;
	coords = { { x - 2, y + 1 }, { x - 1, y + 2 }, { x + 1, y + 2 }, { x + 2, y + 1 },
			   { x - 2, y - 1 }, { x - 1, y - 2 }, { x + 1, y - 2 }, { x + 2, y - 1 } };
}

bool VirtualValidator::underAttack(scoord coord)
{
	// better make it return threatening tile
	// and even better to return all threatening tiles,
	// but only if we need to add arrows for several possible moves like in chess.com

	list <scoord> need_check;
	kingPotential(coord, need_check);
	for (auto move : need_check)
		if (inBoard(move) && differentColor(move) && pieceName(move) == 'K')
			return true;

	knightPotential(coord, need_check);
	for (auto move : need_check)
		if (inBoard(move) && differentColor(move) && pieceName(move) == 'N')
			return true;

	int x = coord.x, y = coord.y;
	int k = theTurn() ? 1 : -1;
	need_check = { {x - 1, y + k}, {x + 1, y + k} };
	for (auto move : need_check)
		if (inBoard(move) && differentColor(move) && pieceName(move) == 'P')
			return true;

	auto bordersPerp = [&](scoord coord) {
		return inBoard(coord) && (!occupied(coord) || differentColor(coord));
		};
	auto comparePerp = [&](scoord coord) {
		return pieceName(coord) == 'R' || pieceName(coord) == 'Q';
		};
	auto checkPerp = [&](scoord coord, bool& result) -> bool {
		return enemyFinder(coord, comparePerp, bordersPerp, result);
		};
	if (fastLine(coord, checkPerp, perp_dir))
		return true;

	auto compareDiag = [&](scoord coord) {
		return pieceName(coord) == 'B' || pieceName(coord) == 'Q';
		};
	auto checkDiag = [&](scoord coord, bool& result) -> bool {
		return enemyFinder(coord, compareDiag, bordersPerp, result);
		};
	return fastLine(coord, checkDiag, diag_dir);
}

void VirtualValidator::findValid(scoord from)
{
	// NOTE: for the use of this code, we consider that vboard goes [x][y]
	// instead of [i][j] that is equal to [y][x];
	// and we also consider that white pieces are on the 0 line and black on the 
	// 7 line
	//valid_moves.clear();
	auto from_tile = theTile(from);
	const bool turn = theTurn();
	const char piece = from_tile.piece_name;
	const scoord king = turn ? theWKing().coord : theBKing().coord;
	int X, Y;
	bool first_evaluation = true, may_exposure = false;
	scoord add;
	list <scoord> potenial_moves;


	// Direction cycles:
	auto runThrough = [&](scoord coord, scoord add, lambda stop_cond, lambda borders_cond)
		{
			// here range is (from, to] or (from, to), depending on stop_cond
			coord.x += add.x, coord.y += add.y;
			while (borders_cond(coord)) {
				if (stop_cond(coord))
					return true;
				coord.x += add.x, coord.y += add.y;
			}
			return false;
		};
	auto onDiagonals = [&](scoord coord, lambda stop_cond, lambda borders_cond)
		{
			bool res = false;
			for (scoord dir : diag_dir)
				res |= runThrough(coord, dir, stop_cond, borders_cond);
			return res;
		};
	auto onPerp = [&](scoord coord, lambda stop_cond, lambda borders_cond)
		{
			bool res = false;
			for (scoord dir : perp_dir)
				res |= runThrough(coord, dir, stop_cond, borders_cond);
			return res;
		};

	// Usefull formulas:
	auto findDirection = [](scoord beg, scoord end) -> scoord {
		// if two tiles are on the same diagonal or perpendicular
		// it will return unit vector, which can be used as increments for cycles, 
		// otherwise it's undefined behavior
		int X = end.x - beg.x;
		int Y = end.y - beg.y;
		scoord add{ X > 0 ? 1 : -1,  Y > 0 ? 1 : -1 };
		if (!Y) add.y = 0;
		if (!X) add.x = 0;
		return add;
		};
	auto notAlignsKing = [&](scoord coord, scoord king, scoord from) {
		// checks that "coord" tile is not on the same line with "king" tile and "from"
		// tile, with the least operations possible
		return (from.x - coord.x) * (coord.y - king.y) !=
			(from.y - coord.y) * (coord.x - king.x);
		};

	// Lambdas about king:
	auto exposureKing = [&](scoord coord) -> bool {
		if (first_evaluation) {
			X = from.x - king.x,
				Y = from.y - king.y;
			bool from_aligns_king = abs(X) == abs(Y) || !X || !Y;
			if (from_aligns_king) {
				add = findDirection(king, from);
				auto bordersBetween = [king, from](scoord coord) -> bool {
					return coord != king && coord != from;
					// whithout "if(from_align_king)"
					// checking this will lead to borders violation sometimess (only if "from" tile
					// is not aligned with "king" tile), e.g. for knight; be carefull
					// also cycle may potentially leave the borders if from is equal king 
					// (the moving piece is king)
					};
				bool nothing_between = !runThrough(king, add, occupied, bordersBetween);
				auto threatensToKing = [&](scoord coord) -> bool {
					char name = pieceName(coord);
					return (abs(X) == abs(Y)
						? differentColor(coord) && (name == 'B' || name == 'Q')
						: differentColor(coord) && (name == 'R' || name == 'Q'));
					};
				auto bordersAfter = [&](scoord coord) {
					return inBoard(coord) && (!occupied(coord) || differentColor(coord));
					};
				auto checkerAfter = [&](scoord coord, bool& result) -> bool {
					return enemyFinder(coord, threatensToKing, bordersAfter, result);
					};
				bool occurs_threat;
				fastThrough(from, add, checkerAfter, occurs_threat);
				may_exposure = from_aligns_king && nothing_between && occurs_threat;
			}
			first_evaluation = false;
		}
		return may_exposure && notAlignsKing(coord, king, from); // if not aligns king then exposure him
		// otherwise it will protect king with its own body
		};
	auto letKingDie = [&](scoord coord) {
		// FIX: actually canPass() check should be in exposureKing() logically,
		// but it will cause an exact copy of this function in eposureKing()
		if (check || canPass(from, coord)) {
			vove virtual_move;
			moveVirtually(from, coord, virtual_move);
			bool check_remains =
				pieceName(coord) == 'K' ? underAttack(coord) : underAttack(king);
			revertVirtualMove(virtual_move);
			return check_remains;
		}
		return false;
		};
	auto castlingPotential = [&](list<scoord>& coords) {
		// adds coords of potential castling destination for king to the list
		if (turn)
			coords = { {2, 0}, {6, 0} };
		else
			coords = { {2, 7}, {6, 7} };
		};

	// Checker conditions:
	// For knight, rook, bishop and queen
	auto canMoveTo = [&](scoord coord) -> bool {
		return inBoard(coord) && (!occupied(coord) || differentColor(coord)) &&
			pieceName(coord) != 'K' && !exposureKing(coord);
		};
	// For knight
	auto canMoveKnightTo = [&](scoord coord) {
		return inBoard(coord) && (!occupied(coord) || differentColor(coord)) &&
			pieceName(coord) != 'K' && !exposureKing(coord) && !letKingDie(coord);
		};
	// For king
	auto canMoveKingTo = [&](scoord coord) -> bool {
		return inBoard(coord) && (!occupied(coord) || differentColor(coord)) &&
			pieceName(coord) != 'K' && !underAttack(coord) && !letKingDie(coord);
		};
	// For pawn
	auto pawnCanEat = [&](scoord coord) -> bool {
		return inBoard(coord) && !exposureKing(coord) && !letKingDie(coord) &&
			(!occupied(coord) && canPass(from, coord) || differentColor(coord) &&
				pieceName(coord) != 'K');
		};
	auto pawnCanMove = [&](scoord coord) -> bool {
		return inBoard(coord) && !occupied(coord) && !exposureKing(coord);
		};

	// Adding coords to this->valid_moves;
	auto addMove = [&](scoord coord) -> bool {  // FIX: should be reread
		bool let_die;
		if (canMoveTo(coord)) {
			let_die = letKingDie(coord);
			if (!let_die)
				addValid(coord);
			// don't add to valid_moves, but continue the cycle,
			// threat may be somewhere next on the line
			if (occupied(coord))
				return true; // add move but stop, we can eat but cannot go next
			return false; // continue
		}
		else
			return true; // break cycle
		};
	auto addPawnMoves = [&](scoord coord) -> void {
		int x = coord.x, y = coord.y;
		scoord move;
		int k = turn ? 1 : -1;
		move = { x, y + 1 * k };
		if (pawnCanMove(move)) {
			if (!letKingDie(move))
				addValid(move);
			move = { x, y + 2 * k };
			if ((turn ? from.y == 1 : from.y == 6) &&
				pawnCanMove(move) && !letKingDie(move))
				addValid(move);
		}
		move = { x - 1, y + 1 * k };
		if (pawnCanEat(move) && !letKingDie(move))
			addValid(move);
		move = { x + 1, y + 1 * k };
		if (pawnCanEat(move) && !letKingDie(move))
			addValid(move);
		};

	switch (piece)
	{
	case 'P':  // pawn
		addPawnMoves(from);
		break;
	case 'N':  // knight
		knightPotential(from, potenial_moves);
		for (auto coord : potenial_moves)
			if (canMoveKnightTo(coord))
				addValid(coord);
		break;
	case 'K':  // king
	{
		kingPotential(from, potenial_moves);
		for (auto coord : potenial_moves)
			if (canMoveKingTo(coord))
				addValid(coord);
		castlingPotential(potenial_moves);
		scoord rook_stub;
		for (auto coord : potenial_moves)
			if (canCastle(from, coord, rook_stub))
				addValid(coord);
		break;
	}
	case 'B':  // bishop
		onDiagonals(from, addMove, inBoard);
		break;
	case 'R':  // rook
		onPerp(from, addMove, inBoard);
		break;
	case 'Q':  // queen
		onPerp(from, addMove, inBoard);
		onDiagonals(from, addMove, inBoard);
	};
}

bool VirtualValidator::isValid(scoord coord)
{
	return valid_moves.count(coord);
}

bool VirtualValidator::empty()
{
	return valid_moves.empty();
}

bool VirtualValidator::inCheck(bool color)
{
	auto king = color ? theWKing() : theBKing(); // FIX: how auto will behave here with white_king type Tile*
	return (check = underAttack(king.coord));
}

bool VirtualValidator::inCheckmate(bool color)
{
	auto king = color ? theWKing() : theBKing();
	return (check = underAttack(king.coord)) && inStalemate(king.piece_color);
}

bool VirtualValidator::inStalemate(bool color)
{
	movable_pieces.clear();
	valid_moves.clear();
	bool stalemate = true;
	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			scoord coord{ x, y };
			if (occupied(coord) && !differentColor(coord)) {
				findValid(coord);
				if (!empty()) {
					valid_moves.clear();
					movable_pieces.insert(coord);
					stalemate = false;
				}
			}
		}
	}
	return stalemate;
}

bool VirtualValidator::canCastle(scoord from, scoord to, scoord& rook)
{
	list<int> castling_side;
	if (theTurn() && theTile(from).piece_name == 'K' && !has_moved[1])
		castling_side = { 0, 2 };
	else if (!theTurn() && theTile(from).piece_name == 'K' && !has_moved[4])
		castling_side = { 3, 5 };
	for (int i : castling_side) {
		if (to == castling_destination[i] && !has_moved[i]) {
			for (auto coord : should_be_free[i])
				if (theTile(coord).piece_name != 'e')
					return false;
			for (auto coord : should_be_safe[i])
				if (underAttack(coord))
					return false;
			rook = rooks_kings[i];
			return true;
		}
	}
	return false;
}

bool VirtualValidator::canPass(scoord from, scoord to)
{
	if (theStory().empty())
		return false;
	vove last_move = theStory().back().move;
	VirtualTile opp_from_tile = last_move.first;
	VirtualTile opp_to_tile = last_move.second;
	scoord opp_from = opp_from_tile.coord;
	scoord opp_to = opp_from_tile.coord;
	VirtualTile from_tile = theTile(from);
	VirtualTile to_tile = theTile(to);
	return (opp_from_tile.piece_name == 'P' &&
		opp_from_tile.piece_color != from_tile.piece_color &&
		abs(opp_to.y - opp_from.y) == 2 &&
		opp_from.x == to.x &&
		from.y == opp_to.y);
}

bool VirtualValidator::canPromote(scoord pawn, scoord destination)
{
	return theTile(pawn).piece_name == 'P' && destination.y == (theTurn() ? 7 : 0);
}

void VirtualValidator::reactOnMove(scoord from, scoord to)
{
	for (int i = 0; i < 6; i++)
		if (from == rooks_kings[i] || to == rooks_kings[i])
			has_moved[i] = true;
}

void VirtualValidator::bringBack(scoord from, scoord to)
{
	for (int i = 0; i < 6; i++)
		if (from == rooks_kings[i] || to == rooks_kings[i])
			has_moved[i] = false;
}

//qint64 VirtualValidator::countMovesTest(int depth, int i)
//{
//	static bool initial_turn_copy = theTurn();
//	static VirtualTile tiles_copy[8][8];
//	qint64 particular_move_count = 0;
//	if (i == 0)
//	{
//		initial_turn_copy = theTurn();
//		particular_move_count = 0;
//		for (int x = 0; x < 8; x++)
//			for (int y = 0; y < 8; y++)
//				tiles_copy[x][y] = vboard[x][y]->toVirtu();
//	}
//	if (i < depth)
//	{
//		for (int x = 0; x < 8; x++)
//		{
//			for (int y = 0; y < 8; y++)
//			{
//				scoord coord = { x, y };
//				auto tile = theTile({ x, y });
//				if (occupied(coord) && !differentColor(coord))
//				{
//					findValid(tile);
//					if (i == depth - 1)
//						particular_move_count += valid_moves.size();
//					auto valid_moves_copy = valid_moves;
//					valid_moves.clear();
//					if (i != depth - 1)
//					{
//						for (auto move : valid_moves_copy)
//						{
//							vove last_move;
//							board.moveVirtually(tile, move, last_move);
//							board.turn = !board.turn;
//							inCheck(board.turn);
//							reactOnMove(tile.coord, move.coord);
//							auto mc = countMovesTest(depth, i + 1);
//							if (i == 0)
//							{
//								//                                qDebug() << char('a' + x) +
//								//                                            QString::number(y + 1) +
//								//                                            char('a' + move.coord.x) +
//								//                                            QString::number(move.coord.y + 1)
//								//                                         << "-"
//								//                                         << mc;
//								//                                qDebug()
//								//                                         << "From"
//								//                                         << char('a' + x) + QString::number(y + 1)
//								//                                         << "to"
//								//                                         << char('a' + move.coord.x) + QString::number(move.coord.y + 1)
//								//                                         << "-"
//								//                                         << mc;
//							}
//							particular_move_count += mc;
//							board.revertVirtualMove(last_move);
//							board.turn = !board.turn;
//						}
//					}
//				}
//			}
//		}
//	}
//	if (i == 0) {
//		for (int x = 0; x < 8; x++) {
//			for (int y = 0; y < 8; y++) {
//				board.restoreTile(tiles_copy[x][y]);
//			}
//		}
//		board.turn = initial_turn_copy;
//	}
//	return particular_move_count;
//}